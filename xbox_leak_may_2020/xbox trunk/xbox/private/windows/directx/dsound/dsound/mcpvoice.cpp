/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mcpvoice.cpp
 *  Content:    MCP-X audio device objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  01/09/01    dereks  Created based on NVidia/georgioc code.
 *
 ****************************************************************************/

#include "dsoundi.h"


/****************************************************************************
 *
 *  CMcpxVoiceClient
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      CMcpxAPU * [in]: pointer back to the core APU object.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::CMcpxVoiceClient"

DWORD CMcpxVoiceClient::m_dwStuckVoiceCount = 0;

CMcpxVoiceClient::CMcpxVoiceClient
(
    CMcpxAPU *                  pMcpxApu,
    CDirectSoundVoiceSettings * pSettings
)
{
    DWORD                   i;
    
    DPF_ENTER();
    
    m_pMcpxApu = ADDREF(pMcpxApu);
    m_pSettings = ADDREF(pSettings);

    //
    // Initialize defaults
    //

    m_dw3dMode = DS3DMODE_NORMAL;

    InitializeListHead(&m_leActiveVoice);
    InitializeListHead(&m_lePendingInactiveVoice);
    InitializeListHead(&m_leSourceVoice);
    InitializeListHead(&m_lstSourceVoices);

    m_bVoiceList = MCPX_VOICELIST_INVALID;

    for(i = 0; i < NUMELMS(m_ahVoices); i++)
    {
        m_ahVoices[i] = MCPX_VOICE_HANDLE_INVALID;
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CMcpxVoiceClient
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
#define DPF_FNAME "CMcpxVoiceClient::~CMcpxVoiceClient"

CMcpxVoiceClient::~CMcpxVoiceClient
(
    void
)
{
    DPF_ENTER();

    ASSERT(!(m_dwStatus & ~MCPX_VOICESTATUS_RELEASEMASK));

    //
    // Make sure we're not still in the active voice list
    //

    AssertValidEntryList(&m_leActiveVoice, ASSERT_NOT_IN_LIST);
    AssertValidEntryList(&m_lePendingInactiveVoice, ASSERT_NOT_IN_LIST);
    
    //
    // Make sure we're disconnected from any submixes
    //

    DisconnectVoice();

    AssertValidEntryList(&m_leSourceVoice, ASSERT_NOT_IN_LIST);
    AssertValidEntryList(&m_lstSourceVoices, ASSERT_NOT_IN_LIST);

    //
    // Release object references
    //
    
    DELETE(m_pHrtfSource);
    DELETE(m_pI3dl2Source);

    RELEASE(m_pSettings);
    RELEASE(m_pMcpxApu);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      CMcpxVoiceClientSettings * [in]: shared voice settings.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::Initialize"

HRESULT 
CMcpxVoiceClient::Initialize
(
    BOOL                    fStream
)
{
    HRESULT                 hr  = DS_OK;
    
    DPF_ENTER();

    ASSERT(!m_RegCache.CfgFMT);
    ASSERT(!m_RegCache.CfgMISC);
    
    //
    // Setup voice for buffer or stream mode
    //

    if(fStream)
    {
        m_RegCache.CfgFMT |= MCPX_MAKE_REG_VALUE(NV1BA0_PIO_SET_VOICE_CFG_FMT_DATA_TYPE_STREAM, NV1BA0_PIO_SET_VOICE_CFG_FMT_DATA_TYPE);
    }
    else
    {
        m_RegCache.CfgFMT |= MCPX_MAKE_REG_VALUE(NV1BA0_PIO_SET_VOICE_CFG_FMT_DATA_TYPE_BUFFER, NV1BA0_PIO_SET_VOICE_CFG_FMT_DATA_TYPE);
    }

    //
    // Set MIXIN-only values
    //

    if(m_pSettings->m_dwFlags & DSBCAPS_MIXIN)
    {
        m_RegCache.CfgFMT |= MCPX_MAKE_REG_VALUE(DSMIXBIN_SUBMIX, NV_PAVS_VOICE_CFG_FMT_MULTIPASS_BIN__ALIAS__);
        m_RegCache.CfgFMT |= MCPX_MAKE_REG_VALUE(NV1BA0_PIO_SET_VOICE_CFG_FMT_MULTIPASS_ON, NV1BA0_PIO_SET_VOICE_CFG_FMT_MULTIPASS);
        m_RegCache.CfgFMT |= MCPX_MAKE_REG_VALUE(NV_PAVS_VOICE_CFG_FMT_CLEAR_MIX_TRUE, NV_PAVS_VOICE_CFG_FMT_CLEAR_MIX);
    }

    //
    // Turn interrupts on for streams.  Buffers will get them from the idle
    // voice handler, not the hardware.
    //

    if(fStream)
    {
        m_RegCache.CfgMISC |= MCPX_MAKE_REG_VALUE(NV1BA0_PIO_SET_VOICE_CFG_MISC_NOTIFY_INTERRUPT_ENABLE, NV1BA0_PIO_SET_VOICE_CFG_MISC_NOTIFY_INTERRUPT);
    }

    //
    // Set up filter and SRC headroom
    //

    if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
    {
        m_RegCache.CfgFMT |= MCPX_MAKE_REG_VALUE(NV_PAVS_VOICE_CFG_FMT_HEADROOM_SRC0_FLT0, NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM);
    }
    else
    {
        m_RegCache.CfgFMT |= MCPX_MAKE_REG_VALUE(NV_PAVS_VOICE_CFG_FMT_HEADROOM_SRC1_FLT3, NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM);
    }

    //
    // Set the voice format.  If we're a MIXIN voice, we just need to set the
    // voice count to 1.
    //

    if(m_pSettings->m_dwFlags & DSBCAPS_MIXIN)
    {
        ASSERT(1 == m_pSettings->m_fmt.nChannels);
        m_bVoiceCount = 1;
    }
    else
    {
        hr = SetFormat();
    }

#ifndef MCPX_BOOT_LIB

    //
    // Allocate 3D helper objects
    //

    if(SUCCEEDED(hr) && (m_pSettings->m_dwFlags & DSBCAPS_CTRL3D))
    {
        ASSERT(m_pSettings->m_p3dParams);
        
        hr = HRFROMP(m_pHrtfSource = NEW(CHrtfSource(*m_pMcpxApu, *m_pSettings->m_p3dParams)));

        if(SUCCEEDED(hr))
        {
            hr = HRFROMP(m_pI3dl2Source = NEW(CI3dl2Source(*m_pMcpxApu, m_pSettings->m_p3dParams->I3dl2Params)));
        }
    }

#endif // MCPX_BOOT_LIB

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetFormat
 *
 *  Description:
 *      Sets the voice format.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::SetFormat"

HRESULT 
CMcpxVoiceClient::SetFormat
(
    void
)
{
    HRESULT                 hr              = DS_OK;
    BYTE                    bVoiceCount;
    DWORD                   i;
    
    DPF_ENTER();
    AutoIrql();

    ASSERT(!(m_pSettings->m_dwFlags & DSBCAPS_MIXIN));

    //
    // Update the number of hardware voices used by the object
    // 

    bVoiceCount = ((m_pSettings->m_fmt.nChannels - 1) >> 1) + 1;

    if(bVoiceCount != m_bVoiceCount)
    {
        if(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED)
        {
            DPF_ERROR("Can't change the number of hardware voices used by an allocated voice");
            hr = DSERR_INVALIDCALL;
        }
        else
        {
            m_bVoiceCount = bVoiceCount;
        }
    }

    //
    // Create the format mask
    //

    if(SUCCEEDED(hr))
    {
        switch(m_pSettings->m_fmt.wFormatTag)
        {
            case WAVE_FORMAT_PCM:
                switch(m_pSettings->m_fmt.wBitsPerSample)
                {
                    case 8:  
                        m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE_U8, NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE);
                        m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE_B8, NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE);
                        break;

                    case 16: 
                        m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE_S16, NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE);
                        m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE_B16, NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE);
                        break;

                    case 24:
                        m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE_S24, NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE);
                        m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE_B32, NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE);
                        break;

                    case 32: 
                        m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE_S32, NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE);
                        m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE_B32, NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE);
                        break;

                    default:
                        ASSERTMSG("Unexpected sample size");
                        break;
                }

                break;

            case WAVE_FORMAT_XBOX_ADPCM:
                m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE_ADPCM, NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE);
                m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE_ADPCM, NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE);
                break;

            default:
                ASSERTMSG("Unexpected format type");
                break;
        }
    }

    //
    // Set samples per block.  This register overlaps with the multipass bin 
    // alias for mixin buffers, so we can't set both.
    //

    if(SUCCEEDED(hr))
    {
        m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, m_pSettings->m_fmt.nChannels - 1, NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLES_PER_BLOCK);
    }

    //
    // If the voice uses more than 1 channel, enable stereo.  We can do
    // this when we're using multiple hardware voices per software voice
    // because we're restricted to 1, 2, 4 or 6-channels per voice, so 
    // there's only one odd-numbered voice.
    //

    if(SUCCEEDED(hr))
    {
        if(1 == m_pSettings->m_fmt.nChannels)
        {
            m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_STEREO_DISABLE, NV1BA0_PIO_SET_VOICE_CFG_FMT_STEREO);
        }
        else
        {
            ASSERT(!(m_pSettings->m_fmt.nChannels % 2));

            m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_STEREO_ENABLE, NV1BA0_PIO_SET_VOICE_CFG_FMT_STEREO);
        }
    }

    //
    // Update voice registers
    //
    
    if(SUCCEEDED(hr) && (m_dwStatus & MCPX_VOICESTATUS_ALLOCATED))
    {
        MCPX_CHECK_VOICE_FIFO(2 * m_bVoiceCount);

        for(i = 0; i < m_bVoiceCount; i++)
        {
            MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);
            MCPX_VOICE_WRITE(SetVoiceCfgFMT, GetVoiceCfgFMT(i));
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  AllocateVoiceResources
 *
 *  Description:
 *      Allocates hardware resources for the voice.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::AllocateVoiceResources"

HRESULT 
CMcpxVoiceClient::AllocateVoiceResources
(
    void
)
{
    HRESULT                 hr              = DS_OK;
    DWORD                   dwVoiceBins;
    DWORD                   dwPitch;
    MCPX_VOICE_VOLUME       Volume;
    DWORD                   i;
    
    DPF_ENTER();
    AutoIrql();

    if(!(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED))
    {
        ASSERT(!(m_dwStatus & ~MCPX_VOICESTATUS_DEACTIVATEMASK));
        
        //
        // Allocate voices
        //

        hr = m_pMcpxApu->AllocateVoices(this);

        //
        // Initialize the notifier wrapper
        //

        if(SUCCEEDED(hr))
        {
            m_Notifier.Initialize(m_ahVoices[0]);
        }

        //
        // Flag the voice as having resources allocated
        //

        if(SUCCEEDED(hr))
        {
            or(&m_dwStatus, MCPX_VOICESTATUS_ALLOCATED);
        }

        //
        // Build mixbin register values
        //

        if(SUCCEEDED(hr))
        {
            ConvertMixBinValues(&dwVoiceBins, &m_RegCache.CfgFMT);
        }

        //
        // Build volume register values
        //

        if(SUCCEEDED(hr))
        {
            ConvertVolumeValues(&Volume);
        }

        //
        // Calculate pitch shift
        //

        if(SUCCEEDED(hr))
        {
            ConvertPitchValue(&dwPitch);
        }

        //
        // Set voice registers
        //

        if(SUCCEEDED(hr))
        {
            for(i = 0; i < m_bVoiceCount; i++)
            {
                MCPX_CHECK_VOICE_FIFO(18);

                MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);
                MCPX_VOICE_WRITE(SetVoiceCfgFMT, GetVoiceCfgFMT(i));
                MCPX_VOICE_WRITE(SetVoiceCfgMISC, m_RegCache.CfgMISC);
                MCPX_VOICE_WRITE(SetVoiceCfgENV0, m_RegCache.CfgENV0);
                MCPX_VOICE_WRITE(SetVoiceCfgENVA, m_RegCache.CfgENVA);
                MCPX_VOICE_WRITE(SetVoiceCfgENV1, m_RegCache.CfgENV1);
                MCPX_VOICE_WRITE(SetVoiceCfgENVF, m_RegCache.CfgENVF);
                MCPX_VOICE_WRITE(SetVoiceCfgLFODLY, m_RegCache.CfgLFODLY);
                MCPX_VOICE_WRITE(SetVoiceLFOENV, m_RegCache.LFOENV);
                MCPX_VOICE_WRITE(SetVoiceLFOMOD, m_RegCache.LFOMOD);
                MCPX_VOICE_WRITE(SetVoiceTarFCA, m_RegCache.TarFCA);
                MCPX_VOICE_WRITE(SetVoiceTarFCB, m_RegCache.TarFCB);
                MCPX_VOICE_WRITE(SetVoiceCfgVBIN, dwVoiceBins);
                MCPX_VOICE_WRITE(SetVoiceTarVOLA, Volume.TarVOLA[i]);
                MCPX_VOICE_WRITE(SetVoiceTarVOLB, Volume.TarVOLB[i]);
                MCPX_VOICE_WRITE(SetVoiceTarVOLC, Volume.TarVOLC[i]);
                MCPX_VOICE_WRITE(SetVoiceTarPitch, dwPitch);
                MCPX_VOICE_WRITE(SetVoiceTarHRTF, 0xFFFF);
            }
        }

#ifndef MCPX_BOOT_LIB

        //
        // If we're 3D, force a reload of the HRTF filter coefficients
        //

        if(SUCCEEDED(hr) && (m_pSettings->m_dwFlags & DSBCAPS_CTRL3D))
        {
            ASSERT(m_pHrtfSource);

            m_pHrtfSource->GetHrtfFilterPair();

            LoadHrtfFilter();
        }

#endif // MCPX_BOOT_LIB

    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  ReleaseVoiceResources
 *
 *  Description:
 *      Releases hardware resources for the voice.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::ReleaseVoiceResources"

void
CMcpxVoiceClient::ReleaseVoiceResources
(
    void
)
{
    DWORD                   i;
    
    DPF_ENTER();
    AutoIrql();

    ASSERT(!(m_dwStatus & ~(MCPX_VOICESTATUS_ALLOCATED | MCPX_VOICESTATUS_RELEASEMASK)));
    
    if(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED)
    {
        //
        // Clear the voice structure
        //

        for(i = 0; i < m_bVoiceCount; i++)
        {
            ZeroMemory((LPVOID)MCPX_VOICE_STRUCT_BASE(m_ahVoices[i]), NV_PAVS_SIZE);

            MCPX_VOICE_STRUCT_WRITE(m_ahVoices[i], NV_PAVS_VOICE_TAR_PITCH_LINK, m_ahVoices[i]);
        }
        
        //
        // Free the notifier wrapper
        //

        m_Notifier.Free();
        
        //
        // Free the hardware voices
        //

        DPF_INFO("Voice %x (%x) freeing resources", this, m_ahVoices[0]);

        m_pMcpxApu->FreeVoices(this);

        //
        // Reset the status
        //

        and(&m_dwStatus, MCPX_VOICESTATUS_RELEASEMASK);
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetMixBins
 *
 *  Description:
 *      Sets the voice mix bin assignments.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::SetMixBins"

HRESULT
CMcpxVoiceClient::SetMixBins
(
    void
)
{
    DWORD                   dwVoiceBins;
    MCPX_VOICE_VOLUME       Volume;
    DWORD                   i;
    
    DPF_ENTER();
    AutoIrql();

    if(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED)
    {
        ConvertMixBinValues(&dwVoiceBins, &m_RegCache.CfgFMT);
        ConvertVolumeValues(&Volume);

        MCPX_CHECK_VOICE_FIFO(8 * m_bVoiceCount);
        
        for(i = 0; i < m_bVoiceCount; i++)
        {
            MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);
            MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_ON);
            MCPX_VOICE_WRITE(SetVoiceCfgVBIN, dwVoiceBins);
            MCPX_VOICE_WRITE(SetVoiceCfgFMT, GetVoiceCfgFMT(i));
            MCPX_VOICE_WRITE(SetVoiceTarVOLA, Volume.TarVOLA[i]);
            MCPX_VOICE_WRITE(SetVoiceTarVOLB, Volume.TarVOLB[i]);
            MCPX_VOICE_WRITE(SetVoiceTarVOLC, Volume.TarVOLC[i]);
            MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_OFF);
        }
    }
    
    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  ConvertMixBinValues
 *
 *  Description:
 *      Converts a mixbin mask to register values.
 *
 *  Arguments:
 *      LPDWORD [out]: VoiceBin register value.
 *      LPDWORD [out]: VoiceFormat register value.  Only the low 8 bits of
 *                     this value will be modified.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::ConvertMixBinValues"

void
CMcpxVoiceClient::ConvertMixBinValues
(
    LPDWORD                 pdwVoiceBins,
    LPDWORD                 pdwVoiceFormat
)
{
    DWORD                   adwMixBins[8];
    DWORD                   i, a, b;
    
    DPF_ENTER();

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    //
    // The MCP-X registers are stored in a wacky format.  Instead of using a
    // 32-bit register that uses 1 bit per mixbin, they use 5-bit mixbin 
    // indeces that are stored in 30 bits of one register and 10 of another.
    //
    // The hardware gets surly if a voice is assigned to less than 8 mixbins, 
    // so we'll assign it to 8, but set the extra bins' volumes to 0.
    //
    // You'll notice that when we invent mixbins to assign the voice to,
    // they're unique.  This is to prevent a theoretical stall in the VP.
    //

    for(i = 0; i < m_pSettings->m_dwMixBinCount; i++)
    {
        adwMixBins[i] = m_pSettings->m_abMixBins[i];
    }

    a = DSMIXBIN_FIRST;

    while(i < NUMELMS(adwMixBins))
    {
        ASSERT(a <= DSMIXBIN_LAST);
        
        for(b = 0; b < i; b++)
        {
            if(a == adwMixBins[b])
            {
                break;
            }
        }

        if(b >= i)
        {
            adwMixBins[i++] = a;
        }
    
        a++;
    }            

#ifdef DEBUG

    for(i = 0; i < m_pSettings->m_dwMixBinCount; i++)
    {
        DPF_BLAB("Voice %x (%x) assigned to mixbin %lu", this, m_ahVoices[0], adwMixBins[i]);
    }

    for(; i < NUMELMS(adwMixBins); i++)
    {
        DPF_BLAB("Voice %x (%x) assigned to mixbin %lu (unused)", this, m_ahVoices[0], adwMixBins[i]);
    }

#endif // DEBUG

    *pdwVoiceBins = 0;
    
    for(i = 0; i < 3; i++)
    {
        *pdwVoiceBins |= adwMixBins[i] << (i * 5);
    }

    for(; i < 6; i++)
    {
        *pdwVoiceBins |= adwMixBins[i] << ((i * 5) + 1);
    }        

    *pdwVoiceFormat &= ~0x3FF;

    for(; i < 8; i++)
    {
        *pdwVoiceFormat |= adwMixBins[i] << ((i - 6) * 5);
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetVolume
 *
 *  Description:
 *      Sets the voice volume.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::SetVolume"

HRESULT
CMcpxVoiceClient::SetVolume
(
    void
)
{
    MCPX_VOICE_VOLUME       Volume;
    DWORD                   i;
    
    DPF_ENTER();
    AutoIrql();

    if(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED)
    {
        ConvertVolumeValues(&Volume);

        MCPX_CHECK_VOICE_FIFO(6 * m_bVoiceCount);
        
        for(i = 0; i < m_bVoiceCount; i++)
        {
            MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);
            MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_ON);
            MCPX_VOICE_WRITE(SetVoiceTarVOLA, Volume.TarVOLA[i]);
            MCPX_VOICE_WRITE(SetVoiceTarVOLB, Volume.TarVOLB[i]);
            MCPX_VOICE_WRITE(SetVoiceTarVOLC, Volume.TarVOLC[i]);
            MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_OFF);
        }
    }
    
    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  ConvertVolumeValues
 *
 *  Description:
 *      Converts volume values to register format.
 *
 *  Arguments:
 *      LPMCPX_VOICE_VOLUME [out]: volume register data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::ConvertVolumeValues"

void
CMcpxVoiceClient::ConvertVolumeValues
(
    LPMCPX_VOICE_VOLUME     pVolumeRegisters
)
{
    static const DWORD      dwMcpxMaxAttenuation                    = MCPX_GET_REG_VALUE(~0UL, NV_PAVS_VOICE_TAR_VOLA_VOLUME0);
    DWORD                   adwVolumes[MCPX_HW_MAX_VOICE_MIXBINS];

#ifndef MCPX_BOOT_LIB

    BOOL                    f3dVolume                               = FALSE;
    LONG                    l3dVolume;
    LONG                    l3dFrontDirect;
    LONG                    l3dRearDirect;
    LONG                    l3dReverbSend;

#endif // MCPX_BOOT_LIB

    DWORD                   dwPerVoiceBins;
    DWORD                   dwVoiceIndex;
    DWORD                   i;
    
    DPF_ENTER();

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

#ifndef MCPX_BOOT_LIB

    //
    // Calculate generic 3D volume, front direct-path, rear direct-path 
    // and reverb send levels separately.  3D volume will be applied
    // to all mixbins, front to the fronts, rear to the rears and reverb
    // to the I3DL2 mixbin.
    //

    if(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D)
    {
        ASSERT(m_pSettings->m_p3dParams);

        if(DS3DMODE_DISABLE != m_pSettings->m_p3dParams->HrtfParams.dwMode)
        {
            ASSERT(m_pHrtfSource);
        
            ASSERT(m_pHrtfSource->m_3dVoiceData.lDistanceVolume <= 0);
            ASSERT(m_pHrtfSource->m_3dVoiceData.lConeVolume <= 0);
            ASSERT(m_pHrtfSource->m_3dVoiceData.lFrontVolume <= 0);
            ASSERT(m_pHrtfSource->m_3dVoiceData.lRearVolume <= 0);

            l3dVolume = m_pHrtfSource->m_3dVoiceData.lDistanceVolume + m_pHrtfSource->m_3dVoiceData.lConeVolume;
        
            l3dFrontDirect = l3dVolume + m_pHrtfSource->m_3dVoiceData.lFrontVolume + m_pI3dl2Source->m_I3dl2Data.lDirect;
            l3dFrontDirect = min(l3dFrontDirect, 0);

            l3dRearDirect = l3dVolume + m_pHrtfSource->m_3dVoiceData.lRearVolume + m_pI3dl2Source->m_I3dl2Data.lDirect;
            l3dRearDirect = min(l3dRearDirect, 0);

            l3dReverbSend = l3dVolume + m_pI3dl2Source->m_I3dl2Data.lSource;
            l3dReverbSend = min(l3dReverbSend, 0);

            f3dVolume = TRUE;
        }
    }

#endif // MCPX_BOOT_LIB

    //
    // If we're multichannel, determine which mixbins need to be muted.
    // We need to be sure that since all of our hardware voices point to
    // the same mixbins, we mute some on a per-voice basis so we don't
    // end up with multiple voices feeding the same mixbin.
    //

    ASSERT(!(m_pSettings->m_dwMixBinCount % m_bVoiceCount));
    dwPerVoiceBins = m_pSettings->m_dwMixBinCount / m_bVoiceCount;

    //
    // Enter the per-voice loop
    //

    for(dwVoiceIndex = 0; dwVoiceIndex < m_bVoiceCount; dwVoiceIndex++)
    {
        //
        // For each mixbin we're assigned to, set a volume in the array
        //

        for(i = 0; i < NUMELMS(adwVolumes); i++)
        {
            //
            // Are we using this mixbin?
            //

            if((i < (dwPerVoiceBins * dwVoiceIndex)) || (i >= (dwPerVoiceBins * (dwVoiceIndex + 1))))
            {
                adwVolumes[i] = dwMcpxMaxAttenuation;
                continue;
            }

            //
            // Start with global voice volume
            //

            ASSERT(m_pSettings->m_lVolume <= 0);
            adwVolumes[i] = -m_pSettings->m_lVolume;

            //
            // Combine mixbin-specific volume with global voice volume
            //

            ASSERT(m_pSettings->m_alMixBinVolumes[m_pSettings->m_abMixBins[i]] <= 0);
            adwVolumes[i] -= m_pSettings->m_alMixBinVolumes[m_pSettings->m_abMixBins[i]];

#ifndef MCPX_BOOT_LIB

            //
            // Add 3D attenuation
            //

            if(f3dVolume)
            {
                switch(m_pSettings->m_abMixBins[i])
                {
                    case DSMIXBIN_3D_FRONT_LEFT:
                    case DSMIXBIN_3D_FRONT_RIGHT:
                        ASSERT(l3dFrontDirect <= 0);
                        adwVolumes[i] -= l3dFrontDirect;
                        break;

                    case DSMIXBIN_3D_BACK_LEFT:
                    case DSMIXBIN_3D_BACK_RIGHT:
                        ASSERT(l3dRearDirect <= 0);
                        adwVolumes[i] -= l3dRearDirect;
                        break;

                    case DSMIXBIN_I3DL2:
                        ASSERT(l3dReverbSend <= 0);
                        adwVolumes[i] -= l3dReverbSend;
                        break;

                    default:
                        ASSERT(l3dVolume <= 0);
                        adwVolumes[i] -= l3dVolume;
                        break;
                }

                if(m_pHrtfSource->m_3dVoiceData.dwMixBinValidMask & (1UL << m_pSettings->m_abMixBins[i]))
                {
                    ASSERT(m_pHrtfSource->m_3dVoiceData.alMixBinVolumes[m_pSettings->m_abMixBins[i]] <= 0);
                    adwVolumes[i] -= m_pHrtfSource->m_3dVoiceData.alMixBinVolumes[m_pSettings->m_abMixBins[i]];
                }
            }

#endif // MCPX_BOOT_LIB

            //
            // Convert to register format
            //

            DPF_BLAB("Voice %x (%x) mixbin %lu (%lu) volume -%ld", this, m_ahVoices[dwVoiceIndex], i, m_pSettings->m_abMixBins[i], adwVolumes[i]);
            
            adwVolumes[i] <<= 6;
            adwVolumes[i] /= 100;

            CHECKRANGE(adwVolumes[i], 0, dwMcpxMaxAttenuation);
        }

        //
        // Set register values
        //

        pVolumeRegisters->TarVOLA[dwVoiceIndex] = MCPX_MAKE_REG_VALUE(adwVolumes[0], NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME0);
        pVolumeRegisters->TarVOLA[dwVoiceIndex] |= MCPX_MAKE_REG_VALUE(adwVolumes[1], NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME1);
        pVolumeRegisters->TarVOLA[dwVoiceIndex] |= MCPX_MAKE_REG_VALUE(adwVolumes[6], NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME6_B3_0);
        pVolumeRegisters->TarVOLA[dwVoiceIndex] |= MCPX_MAKE_REG_VALUE(adwVolumes[7], NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME7_B3_0);

        pVolumeRegisters->TarVOLB[dwVoiceIndex] = MCPX_MAKE_REG_VALUE(adwVolumes[2], NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME2);
        pVolumeRegisters->TarVOLB[dwVoiceIndex] |= MCPX_MAKE_REG_VALUE(adwVolumes[3], NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME3);
        pVolumeRegisters->TarVOLB[dwVoiceIndex] |= MCPX_MAKE_REG_VALUE(adwVolumes[6] >> 4, NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME6_B7_4);
        pVolumeRegisters->TarVOLB[dwVoiceIndex] |= MCPX_MAKE_REG_VALUE(adwVolumes[7] >> 4, NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME7_B7_4);

        pVolumeRegisters->TarVOLC[dwVoiceIndex] = MCPX_MAKE_REG_VALUE(adwVolumes[4], NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME4);
        pVolumeRegisters->TarVOLC[dwVoiceIndex] |= MCPX_MAKE_REG_VALUE(adwVolumes[5], NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME5);
        pVolumeRegisters->TarVOLC[dwVoiceIndex] |= MCPX_MAKE_REG_VALUE(adwVolumes[6] >> 8, NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME6_B11_8);
        pVolumeRegisters->TarVOLC[dwVoiceIndex] |= MCPX_MAKE_REG_VALUE(adwVolumes[7] >> 8, NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME7_B11_8);
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetPitch
 *
 *  Description:
 *      Sets the voice pitch.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::SetPitch"

HRESULT
CMcpxVoiceClient::SetPitch
(
    void
)
{
    PLIST_ENTRY             pleSourceVoice;
    CMcpxVoiceClient *      pSourceVoice;
    DWORD                   dwPitch;
    DWORD                   i;
    
    DPF_ENTER();
    AutoIrql();

    //
    // If we're a submix destination, we can't change our pitch.  It has to
    // stay locked at 0.  Instead, we'll update all the source voices' pitch
    // values.
    //

    if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
    {
        for(pleSourceVoice = m_lstSourceVoices.Flink; pleSourceVoice != &m_lstSourceVoices; pleSourceVoice = pleSourceVoice->Flink)
        {
            AssertValidEntryList(pleSourceVoice, ASSERT_IN_LIST);

            pSourceVoice = CONTAINING_RECORD(pleSourceVoice, CMcpxVoiceClient, m_leSourceVoice);

            pSourceVoice->SetPitch();
        }
    }
    else if(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED)
    {
        ConvertPitchValue(&dwPitch);

        MCPX_CHECK_VOICE_FIFO(2 * m_bVoiceCount);
        
        for(i = 0; i < m_bVoiceCount; i++)
        {
            MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);
            MCPX_VOICE_WRITE(SetVoiceTarPitch, dwPitch);
        }
    }
    
    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  ConvertPitchValue
 *
 *  Description:
 *      Converts volume values to register format.
 *
 *  Arguments:
 *      LPDWORD [out]: pitch register.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::ConvertPitchValue"

void
CMcpxVoiceClient::ConvertPitchValue
(
    LPDWORD                 pdwPitch
)
{
    CMcpxBuffer *           pSubMixDestination;
    LONG                    lPitch;
    
    DPF_ENTER();

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    //
    // If we're a submix destination, our pitch must be locked at 0.  In order 
    // to allow callers to set the pitch on submix destination voices, all
    // source voices will know to read from our pitch.
    //

    if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
    {
        lPitch = 0;
    }
    else
    {
        lPitch = m_pSettings->m_lPitch;

#ifndef MCPX_BOOT_LIB

        if(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D)
        {
            ASSERT(m_pSettings->m_p3dParams);
            ASSERT(m_pHrtfSource);
            
            if(DS3DMODE_DISABLE != m_pSettings->m_p3dParams->HrtfParams.dwMode)
            {
                lPitch += m_pHrtfSource->m_3dVoiceData.lDopplerPitch;
            }
        }

#endif // MCPX_BOOT_LIB

        if(pSubMixDestination = GetSubMixDestination())
        {
            lPitch += pSubMixDestination->m_pSettings->m_lPitch;

#ifndef MCPX_BOOT_LIB

            if(pSubMixDestination->m_pSettings->m_dwFlags & DSBCAPS_CTRL3D)
            {
                ASSERT(pSubMixDestination->m_pSettings->m_p3dParams);
                ASSERT(pSubMixDestination->m_pHrtfSource);

                if(DS3DMODE_DISABLE != pSubMixDestination->m_pSettings->m_p3dParams->HrtfParams.dwMode)
                {
                    lPitch += pSubMixDestination->m_pHrtfSource->m_3dVoiceData.lDopplerPitch;
                }
            }

#endif // MCPX_BOOT_LIB

        }

        CHECKRANGE(lPitch, DSBPITCH_MIN, DSBPITCH_MAX);
    }

    *pdwPitch = MCPX_MAKE_REG_VALUE(lPitch, NV1BA0_PIO_SET_VOICE_TAR_PITCH_STEP);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  GetAntecedentVoice
 *
 *  Description:
 *      Determines which voice processing list the voice should be inserted
 *      into, and at which position.
 *
 *  Arguments:
 *      LPDWORD [out]: voice list identifier.
 *      CMcpxVoiceClient ** [out]: antecedent voice.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::GetAntecedentVoice"

void
CMcpxVoiceClient::GetAntecedentVoice
(
    LPBYTE                  pbVoiceList,
    CMcpxVoiceClient **     ppAntecedentVoice
)
{
    CMcpxVoiceClient *      pSubMixDestination;
    PLIST_ENTRY             pleAntecedentVoice;
    CMcpxVoiceClient *      pAntecedentVoice;
    
    DPF_ENTER();

    if(pSubMixDestination = GetSubMixDestination())
    {
        *pbVoiceList = MCPX_VOICELIST_MP;

        AssertValidEntryList(&pSubMixDestination->m_leActiveVoice, ASSERT_IN_LIST);

        pleAntecedentVoice = pSubMixDestination->m_leActiveVoice.Blink;

        if(&m_pMcpxApu->m_alstActiveVoices[MCPX_VOICELIST_MP] == pleAntecedentVoice)
        {
            *ppAntecedentVoice = NULL;
        }
        else
        {
            *ppAntecedentVoice = CONTAINING_RECORD(pleAntecedentVoice, CMcpxVoiceClient, m_leActiveVoice);
        }
    }
    else if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
    {
        *pbVoiceList = MCPX_VOICELIST_MP;
        *ppAntecedentVoice = NULL;
    }
    else if(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D)
    {
        *pbVoiceList = MCPX_VOICELIST_3D;
        *ppAntecedentVoice = NULL;
    }
    else
    {
        *pbVoiceList = MCPX_VOICELIST_2D;
        *ppAntecedentVoice = NULL;
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ActivateVoice
 *
 *  Description:
 *      Starts the voice running.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::ActivateVoice"

void
CMcpxVoiceClient::ActivateVoice
(
    void
)
{
    BYTE                    bVoiceList;
    CMcpxBuffer *           pSubMixDestination;
    CMcpxVoiceClient *      pAntecedentVoice;
    MCPX_VOICE_HANDLE       hAntecedentVoice;
    DWORD                   dwAntecedentVoice;
    DWORD                   dwVoiceOn;
    DWORD                   dwVoicePause;
    CIrql                   irql;
    int                     i;
    
    DPF_ENTER();

    ASSERT(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED);

    if(!(m_dwStatus & MCPX_VOICESTATUS_ACTIVE))
    {

#ifndef MCPX_BOOT_LIB

        if(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D)
        {
            if(!(m_pMcpxApu->m_dwState & MCPX_APUSTATE_GPIMAGE))
            {
                DPF_ERROR("No effects image has been downloaded and 3D sounds are feeding the XTLK mixbins.  Your 3D sounds will be silent");
            }
            else if(DSFX_IMAGELOC_UNUSED == m_pMcpxApu->m_pSettings->m_EffectLocations.dwCrosstalkIndex)
            {
                DPF_ERROR("Crosstalk is not in the current effects image and 3D sounds are feeding the XTLK mixbins.  Your 3D sounds will be silent");
            }
        }

#endif // MCPX_BOOT_LIB

        if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
        {
            ASSERT(!IsListEmpty(&m_lstSourceVoices));
        }
        else
        {
            ASSERT(IsListEmpty(&m_lstSourceVoices));
        }

        //
        // Just because the voice status doesn't include ACTIVE doesn't mean
        // the hardware voice is done.  Block until the voice is really 
        // finished.
        //

        WaitForVoiceOff();

        AssertValidEntryList(&m_leActiveVoice, ASSERT_NOT_IN_LIST);
        AssertValidEntryList(&m_lePendingInactiveVoice, ASSERT_NOT_IN_LIST);

        //
        // Update the status
        //
        
        or(&m_dwStatus, MCPX_VOICESTATUS_ACTIVE);

#ifndef MCPX_BOOT_LIB

        //
        // If we're 3D, make sure all 3D data is up-to-date
        //

        if(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D)
        {
            Commit3dSettings();
        }

#endif // MCPX_BOOT_LIB

        //
        // If we're submixing, go ahead and start the destination playing 
        // now so it's in the list before we try to determine where to insert 
        // ourselves.
        //

        if(pSubMixDestination = GetSubMixDestination())
        {
            ASSERT(pSubMixDestination->m_dwStatus & MCPX_VOICESTATUS_ALLOCATED);
            
            pSubMixDestination->Play(TRUE);
        }

        //
        // Raise IRQL for synchronization
        //

        irql.Raise();
        
        //
        // Wait for the method queue to clear so we know we have enough space
        // in the FIFO for the VoiceOn method.
        //

        MCPX_CHECK_VOICE_FIFO(PIO_METHOD_QUEUE_CLEAR);

        //
        // Block the idle handler in order to prevent the voice list from
        // being modified.
        //

        m_pMcpxApu->BlockIdleHandler();

        //
        // Reset the voice notifiers
        //

        m_Notifier.Reset();

        //
        // Turn on the PERSIST bit so voices are left in the processing list 
        // until we take them out.
        //

        if(m_dwStatus & MCPX_VOICESTATUS_PERSIST)
        {
            m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_PERSIST_ON, NV1BA0_PIO_SET_VOICE_CFG_FMT_PERSIST);
        }
        else
        {
            m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_PERSIST_OFF, NV1BA0_PIO_SET_VOICE_CFG_FMT_PERSIST);
        }

        //
        // Figure out where to insert the voice in the hardware processing 
        // list
        //

        GetAntecedentVoice(&bVoiceList, &pAntecedentVoice);

        if(pAntecedentVoice)
        {
            hAntecedentVoice = pAntecedentVoice->m_ahVoices[pAntecedentVoice->m_bVoiceCount - 1];
            ASSERT(hAntecedentVoice < MCPX_HW_MAX_VOICES);

            dwAntecedentVoice = MCPX_MAKE_REG_VALUE(hAntecedentVoice, NV1BA0_PIO_SET_ANTECEDENT_VOICE_HANDLE);
            dwAntecedentVoice |= MCPX_MAKE_REG_VALUE(NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST_INHERIT, NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST);
        }
        else
        {
            dwAntecedentVoice = MCPX_MAKE_REG_VALUE(NV1BA0_PIO_SET_ANTECEDENT_VOICE_HANDLE_NULL, NV1BA0_PIO_SET_ANTECEDENT_VOICE_HANDLE);
            dwAntecedentVoice |= MCPX_MAKE_REG_VALUE(bVoiceList + 1, NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST);
        }

        //
        // Turn the voice on.  If we're supposed to be paused, do that now
        // as well.  Note that we're turning the voices on in reverse order
        // so they're in the hardware processing list correctly.
        //

        MCPX_CHECK_VOICE_FIFO(7 * m_bVoiceCount);

        for(i = m_bVoiceCount - 1; i >= 0; i--)
        {
            if(pAntecedentVoice)
            {
                DPF_BLAB("Voice %x (%x) inheriting voice list position from voice %x (%x)", this, m_ahVoices[i], pAntecedentVoice, hAntecedentVoice);
            }

            dwVoiceOn = MCPX_SET_REG_VALUE(m_RegCache.VoiceOn, m_ahVoices[i], NV1BA0_PIO_VOICE_ON_HANDLE);
        
            if(m_dwStatus & MCPX_VOICESTATUS_ALLPAUSEDMASK)
            {
                dwVoicePause = MCPX_MAKE_REG_VALUE(NV1BA0_PIO_VOICE_PAUSE_ACTION_STOP, NV1BA0_PIO_VOICE_PAUSE_ACTION);
            }
            else
            {
                dwVoicePause = MCPX_MAKE_REG_VALUE(NV1BA0_PIO_VOICE_PAUSE_ACTION_RESUME, NV1BA0_PIO_VOICE_PAUSE_ACTION);
            }

            dwVoicePause |= MCPX_MAKE_REG_VALUE(m_ahVoices[i], NV1BA0_PIO_VOICE_PAUSE_HANDLE);
        
            MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);
            MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_ON);
            MCPX_VOICE_WRITE(SetVoiceCfgFMT, GetVoiceCfgFMT(i));
            MCPX_VOICE_WRITE(SetAntecedentVoice, dwAntecedentVoice);
            MCPX_VOICE_WRITE(VoiceOn, dwVoiceOn);
            MCPX_VOICE_WRITE(VoicePause, dwVoicePause);
            MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_OFF);
        }

        //
        // Add the voice client object to the active voice list
        //

        if(pAntecedentVoice)
        {
            AssertValidEntryList(&pAntecedentVoice->m_leActiveVoice, ASSERT_IN_LIST);
            InsertHeadList(&pAntecedentVoice->m_leActiveVoice, &m_leActiveVoice);
        }
        else
        {
            ASSERT(bVoiceList < NUMELMS(m_pMcpxApu->m_alstActiveVoices));
            InsertHeadList(&m_pMcpxApu->m_alstActiveVoices[bVoiceList], &m_leActiveVoice);
        }

        //
        // Save the voice list index for later retrieval
        //

        ASSERT(MCPX_VOICELIST_INVALID == m_bVoiceList);
        m_bVoiceList = bVoiceList;

        //
        // Wait for the method queue to clear so we know the VoiceOn method
        // has been processed.
        //

        MCPX_CHECK_VOICE_FIFO(PIO_METHOD_QUEUE_CLEAR);

        //
        // Unblock the idle handler
        //

        m_pMcpxApu->UnblockIdleHandler();

        //
        // Lower IRQL
        //

        irql.Lower();

        //
        // All done
        //

        if(m_dwStatus & MCPX_VOICESTATUS_ALLPAUSEDMASK)
        {
            DPF_INFO("Voice %x (%x) now active in list %x, but paused", this, m_ahVoices[0], (DWORD)bVoiceList);
        }
        else
        {
            DPF_INFO("Voice %x (%x) now active in list %x", this, m_ahVoices[0], (DWORD)bVoiceList);
        }
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  DeactivateVoice
 *
 *  Description:
 *      Stops the voice.
 *
 *  Arguments:
 *      BOOL [in]: TRUE if the voice stopped naturally.  FALSE if we're
 *                 forcibly stopping it.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::DeactivateVoice"

void
CMcpxVoiceClient::DeactivateVoice
(
    BOOL                    fBlock
)
{
    CMcpxBuffer *           pSubMixDestination;
    CMcpxVoiceClient *      pSourceVoice;
    PLIST_ENTRY             pleListEntry;
    BOOL                    fSourceValid;
    CIrql                   irql;
    DWORD                   i;
    
    DPF_ENTER();

    if((m_dwStatus & MCPX_VOICESTATUS_ACTIVEMASK) == MCPX_VOICESTATUS_ACTIVEMASK)
    {
        DPF_INFO("Deactivating voice %x (%x)", this, m_ahVoices[0]);

        //
        // Raise IRQL for synchronization
        //

        irql.Raise();

        //
        // Block the idle handler until we're done setting the voice status
        //

        m_pMcpxApu->BlockIdleHandler();
        
        //
        // Remove pretty much everything from the status mask, since
        // most of the bits are only valid when we're active.
        //

        and(&m_dwStatus, MCPX_VOICESTATUS_DEACTIVATEMASK);

        //
        // Turn off the PERSIST bit so we're guaranteed the voice will be
        // stopped.
        //

        m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_PERSIST_OFF, NV1BA0_PIO_SET_VOICE_CFG_FMT_PERSIST);

        //
        // There also seems to be a bug where a looping voice won't always
        // turn off, so we'll disable the looping bit while we're at it.
        //

        m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP_OFF, NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP);

        //
        // Check the voice list identifier.  If it's -1, the voice has
        // already been removed from the voice list.  We'll set a bit in 
        // the status that tells us that we're still waiting for the voice 
        // to turn off.  When this happens, the ISR will clear the status 
        // bit for us.
        //

        if(m_bVoiceList < MCPX_VOICELIST_COUNT)
        {
            //
            // Turn off the voice.  It won't turn off instantly, but the VoiceOff
            // command will be in the command FIFO, so for all intents, the voice
            // really is off.  The one caveat to this is that you want to watch
            // for the VoiceOff notifier before freeing any memory associated
            // with this voice.  It's possible that the VP could access memory
            // between when the VoiceOff notifier goes into the queue and when
            // the voice is processed.
            //

            DEBUGLOG("VoiceOff %x (%x)", this, m_ahVoices[0]);

            or(&m_dwStatus, MCPX_VOICESTATUS_VOICEOFF);
            ASSERT(m_bVoiceList < MCPX_VOICELIST_COUNT);

#ifndef MCPX_SIMULATE_STUCK_VOICE

#ifdef MCPX_NO_VOICE_OFF

            DWORD LFOENV = MCPX_SET_REG_VALUE(m_RegCache.LFOENV, 1, NV1BA0_PIO_SET_VOICE_LFO_ENV_EA_RELEASERATE);
            
            MCPX_CHECK_VOICE_FIFO(4 * m_bVoiceCount);
        
            for(i = 0; i < m_bVoiceCount; i++)
            {
                MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);
                MCPX_VOICE_WRITE(SetVoiceCfgFMT, GetVoiceCfgFMT(i));
                MCPX_VOICE_WRITE(SetVoiceLFOENV, LFOENV);
                MCPX_VOICE_WRITE(VoiceRelease, m_ahVoices[i]);
            }

#else // MCPX_NO_VOICE_OFF

            MCPX_CHECK_VOICE_FIFO(3 * m_bVoiceCount);
        
            for(i = 0; i < m_bVoiceCount; i++)
            {
                MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);
                MCPX_VOICE_WRITE(SetVoiceCfgFMT, GetVoiceCfgFMT(i));
                MCPX_VOICE_WRITE(VoiceOff, m_ahVoices[i]);
            }

#endif // MCPX_NO_VOICE_OFF

#endif // MCPX_SIMULATE_STUCK_VOICE

        }
        else
        {
            ASSERT(MCPX_VOICELIST_INVALID == m_bVoiceList);
        }

        //
        // Unblock the idle handler
        //

        m_pMcpxApu->UnblockIdleHandler();

        //
        // Save the time we posted the VoiceOff request so we can make
        // the hardware handles it later.
        //
        
        KeQuerySystemTime((PLARGE_INTEGER)&m_rtVoiceOff);

        //
        // Schedule a deferred command to check for a stuck voice
        //

        if(!fBlock)
        {
            ScheduleDeferredCommand(MCPX_DEFERREDCMD_VOICE_CHECKSTUCK, 0, 0);
        }

        //
        // If we're submixing, and we're the last source voice to stop,
        // deactivate the destination voice.
        //

        if(pSubMixDestination = GetSubMixDestination())
        {
            fSourceValid = FALSE;

            for(pleListEntry = pSubMixDestination->m_lstSourceVoices.Flink; pleListEntry != &pSubMixDestination->m_lstSourceVoices; pleListEntry = pleListEntry->Flink)
            {
                AssertValidEntryList(pleListEntry, ASSERT_IN_LIST);

                pSourceVoice = CONTAINING_RECORD(pleListEntry, CMcpxVoiceClient, m_leSourceVoice);

                if((pSourceVoice->m_dwStatus & MCPX_VOICESTATUS_ACTIVEMASK) == MCPX_VOICESTATUS_ACTIVEMASK)
                {
                    fSourceValid = TRUE;
                    break;
                }
            }

            if(!fSourceValid)
            {
                pSubMixDestination->Stop();
            }
        }

        //
        // Lower IRQL
        //

        irql.Lower();
    }
    else if(!(m_dwStatus & MCPX_VOICESTATUS_VOICEOFF))
    {
        //
        // Remove the CHECKSTUCK deferred command from the queue
        //

        RemoveDeferredCommand(MCPX_DEFERREDCMD_VOICE_CHECKSTUCK);
    }

    //
    // Block until the voice actually stops
    //

    if(fBlock)
    {
        WaitForVoiceOff();
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  RemoveIdleVoice
 *
 *  Description:
 *      Removes an idle voice from the processing list.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::RemoveIdleVoice"

void
CMcpxVoiceClient::RemoveIdleVoice
(
    BOOL                    fScheduleDpc
)
{
    static const struct
    {
        DWORD               TVL;
        DWORD               CVL;
        DWORD               NVL;
    } ListRegisters[] =
    {
        { NV_PAPU_TVL2D, NV_PAPU_CVL2D, NV_PAPU_NVL2D },    // MCPX_VOICELIST_2D
        { NV_PAPU_TVL3D, NV_PAPU_CVL3D, NV_PAPU_NVL3D },    // MCPX_VOICELIST_3D
        { NV_PAPU_TVLMP, NV_PAPU_CVLMP, NV_PAPU_NVLMP },    // MCPX_VOICELIST_MP
    };

    CMcpxVoiceClient *      pPrev;
    CMcpxVoiceClient *      pNext;
    DWORD                   dwPrevVoice;
    DWORD                   dwNextVoice;
    DWORD                   dwRegister;
    DWORD                   dwTVL;
    DWORD                   dwCVL;
    DWORD                   dwNVL;
    BOOL                    fFixCVL;
    BOOL                    fFixNVL;
    DWORD                   i;

#ifdef MCPX_ENABLE_ISR_DEBUGGING

    ASSERT((KeGetCurrentIrql() > DISPATCH_LEVEL) || m_pMcpxApu->m_dwVoiceMapLock);

    AssertValidEntryList(&m_leActiveVoice, ASSERT_IN_LIST);
    ASSERT(m_bVoiceList < NUMELMS(ListRegisters));

#endif // MCPX_ENABLE_ISR_DEBUGGING

    //
    // Save the current list register values
    //

    MCPX_REG_READ(ListRegisters[m_bVoiceList].TVL, &dwTVL);
    MCPX_REG_READ(ListRegisters[m_bVoiceList].CVL, &dwCVL);
    MCPX_REG_READ(ListRegisters[m_bVoiceList].NVL, &dwNVL);

#ifdef MCPX_ENABLE_ISR_DEBUGGING

    ASSERT(!(dwTVL & 0xFFFF0000));
    ASSERT(!(dwCVL & 0xFFFF0000));
    ASSERT(!(dwNVL & 0xFFFF0000));

#endif // MCPX_ENABLE_ISR_DEBUGGING

    //
    // Determine the voice's position within the list
    //

    if(&m_pMcpxApu->m_alstActiveVoices[m_bVoiceList] == m_leActiveVoice.Blink)
    {
        pPrev = NULL;
        dwPrevVoice = NV_PAPU_FECV_VALUE_NULL;
    }
    else
    {
        pPrev = CONTAINING_RECORD(m_leActiveVoice.Blink, CMcpxVoiceClient, m_leActiveVoice);
        dwPrevVoice = (DWORD)pPrev->m_ahVoices[pPrev->m_bVoiceCount - 1];
    }

    if(&m_pMcpxApu->m_alstActiveVoices[m_bVoiceList] == m_leActiveVoice.Flink)
    {
        pNext = NULL;
        dwNextVoice = NV_PAPU_FECV_VALUE_NULL;
    }
    else
    {
        pNext = CONTAINING_RECORD(m_leActiveVoice.Flink, CMcpxVoiceClient, m_leActiveVoice);
        dwNextVoice = (DWORD)pNext->m_ahVoices[0];
    }

#ifdef MCPX_ENABLE_ISR_DEBUGGING

    //
    // Validate the list surrounding the voice to be removed
    //
    
    if(pPrev)
    {
        MCPX_VOICE_STRUCT_READ(dwPrevVoice, NV_PAVS_VOICE_TAR_PITCH_LINK, &dwRegister);
        ASSERT((dwRegister & 0xFFFF) == (DWORD)m_ahVoices[0]);
    }
    else
    {
        ASSERT(dwTVL == (DWORD)m_ahVoices[0]);
    }

    for(i = 0; i < (DWORD)m_bVoiceCount - 1; i++)
    {
        MCPX_VOICE_STRUCT_READ(m_ahVoices[i], NV_PAVS_VOICE_TAR_PITCH_LINK, &dwRegister);
        ASSERT((dwRegister & 0xFFFF) == (DWORD)m_ahVoices[i + 1]);
    }

    MCPX_VOICE_STRUCT_READ(m_ahVoices[i], NV_PAVS_VOICE_TAR_PITCH_LINK, &dwRegister);
    ASSERT((dwRegister & 0xFFFF) == dwNextVoice);

#endif // MCPX_ENABLE_ISR_DEBUGGING

    //
    // If we're at the head of the list, fix the TVL register.  If not, 
    // fix up the next voice pointer for the previous voice.
    //

    if(pPrev)
    {
        MCPX_VOICE_STRUCT_READ(dwPrevVoice, NV_PAVS_VOICE_TAR_PITCH_LINK, &dwRegister);

        dwRegister &= 0xFFFF0000;
        dwRegister |= dwNextVoice;

        MCPX_VOICE_STRUCT_WRITE(dwPrevVoice, NV_PAVS_VOICE_TAR_PITCH_LINK, dwRegister);
    }
    else
    {
        dwTVL = dwNextVoice;
        
        MCPX_REG_WRITE(ListRegisters[m_bVoiceList].TVL, dwTVL);
    }

    //
    // Enter the hardware voice loop
    //

    for(i = 0, fFixCVL = FALSE, fFixNVL = FALSE; i < m_bVoiceCount; i++)
    {
        //
        // Check the CVL register
        //

        if(dwCVL == (DWORD)m_ahVoices[i])
        {
            fFixCVL = TRUE;
        }

        //
        // Check the NVL register
        //

        if(dwNVL == (DWORD)m_ahVoices[i])
        {
            fFixNVL = TRUE;
        }

        //
        // Remove the voice from the hardware list by making it point to itself
        //

        MCPX_VOICE_STRUCT_READ(m_ahVoices[i], NV_PAVS_VOICE_TAR_PITCH_LINK, &dwRegister);

        dwRegister &= 0xFFFF0000;
        dwRegister |= (WORD)m_ahVoices[i];
        
        MCPX_VOICE_STRUCT_WRITE(m_ahVoices[i], NV_PAVS_VOICE_TAR_PITCH_LINK, dwRegister);
    }

    //
    // Fix up the CVL and NVL registers
    //

    if(fFixCVL)
    {
        if(pNext)
        {
            dwCVL = dwNextVoice;

            MCPX_VOICE_STRUCT_READ(dwNextVoice, NV_PAVS_VOICE_TAR_PITCH_LINK, &dwRegister);
            
            dwRegister &= 0xFFFF;

            if(NV_PAPU_FECV_VALUE_NULL == dwRegister)
            {
                dwRegister = dwTVL;
            }

            dwNVL = dwRegister;
        }
        else
        {
            dwCVL = NV_PAPU_FECV_VALUE_NULL;
            dwNVL = dwTVL;
        }

        MCPX_REG_WRITE(ListRegisters[m_bVoiceList].CVL, dwCVL);
        MCPX_REG_WRITE(ListRegisters[m_bVoiceList].NVL, dwNVL);
    }
    else if(fFixNVL)
    {
        if(pNext)
        {
            dwNVL = dwNextVoice;
        }
        else
        {
            dwNVL = dwTVL;
        }

        MCPX_REG_WRITE(ListRegisters[m_bVoiceList].NVL, dwNVL);
    }

    //
    // Remove the voice client object from the active voice list
    //

    RemoveEntryList(&m_leActiveVoice);

    //
    // Reset the voice list index
    //

    m_bVoiceList = MCPX_VOICELIST_INVALID;

    //
    // If the voice realizes it's stopped before we do, either because it
    // stopped itself or because the SSL interrupt was completed before
    // the idle voice interrupt, the voice will set the VOICEOFF bit in
    // it's status, meaning that it's waiting for notification from us that
    // it's free to release it's resources.  If this bit is set, just clear
    // it.  If it's not, add the voice to the "pending inactive" list.  During
    // the ISR DPC, we'll walk both the active voice lists and the pending
    // inactive list.  This makes it safe for us to remove the voice from the
    // active voice list before the SSL interrupt is handled.
    // 

    if(m_dwStatus & MCPX_VOICESTATUS_VOICEOFF)
    {
        and(&m_dwStatus, ~MCPX_VOICESTATUS_VOICEOFF);

#ifdef MCPX_ENABLE_ISR_DEBUGGING

        DPF_INFO("Voice %x (%x) is now idle", this, m_ahVoices[0]);

#endif // MCPX_ENABLE_ISR_DEBUGGING

    }
    else
    {
        m_Notifier.SetStatus(MCPX_NOTIFIER_VOICE_OFF, TRUE);
        
        if(fScheduleDpc)
        {
            InsertTailList(&m_pMcpxApu->m_lstPendingInactiveVoices, &m_lePendingInactiveVoice);

            m_pMcpxApu->m_arInterruptStatus[0].FEVoice = TRUE;
        
            m_pMcpxApu->ScheduleApuInterruptDpc();
        }

#ifdef MCPX_ENABLE_ISR_DEBUGGING

        DPF_INFO("Voice %x (%x) is now idle, but pending SSL completion", this, m_ahVoices[0]);

#endif // MCPX_ENABLE_ISR_DEBUGGING

    }
}


/****************************************************************************
 *
 *  RemoveStuckVoice
 *
 *  Description:
 *      Removes an idle voice from the processing list.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::RemoveStuckVoice"

void
CMcpxVoiceClient::RemoveStuckVoice
(
    void
)
{
    BOOL                    fIdle;
    
    DPF_ENTER();
    AutoIrql();

#ifdef MCPX_DEBUG_STUCK_VOICES

    BREAK();

#endif // MCPX_DEBUG_STUCK_VOICES

    m_dwStuckVoiceCount++;
    
    //
    // Idle the voice processor
    //

    fIdle = m_pMcpxApu->IdleVoiceProcessor(TRUE);

    //
    // Remove the stuck voice
    //

    RemoveIdleVoice(FALSE);

    //
    // Turn the VP back on
    //

    if(fIdle)
    {
        m_pMcpxApu->IdleVoiceProcessor(FALSE);
    }

    //
    // Service the voice interrupt that didn't happen
    //

    ServiceVoiceInterrupt();

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ReleaseVoice
 *
 *  Description:
 *      Puts the voice into it's release phase.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::ReleaseVoice"

void
CMcpxVoiceClient::ReleaseVoice
(
    void
)
{
    DWORD                   i;
    
    DPF_ENTER();
    AutoIrql();

    if((m_dwStatus & MCPX_VOICESTATUS_ACTIVEMASK) == MCPX_VOICESTATUS_ACTIVEMASK)
    {
        //
        // Release the voice.  We'll get an interrupt later when the voice
        // actually turns off.  Be sure the PERSIST bit is cleared so the voice
        // turns off at the end of the envelope.
        //
        
        DPF_INFO("Releasing voice %x (%x)", this, m_ahVoices[0]);

        or(&m_dwStatus, MCPX_VOICESTATUS_NOTEOFF);
        
        m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_PERSIST_OFF, NV1BA0_PIO_SET_VOICE_CFG_FMT_PERSIST);

        MCPX_CHECK_VOICE_FIFO(3 * m_bVoiceCount);
        
        for(i = 0; i < m_bVoiceCount; i++)
        {
            MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);
            MCPX_VOICE_WRITE(SetVoiceCfgFMT, GetVoiceCfgFMT(i));
            MCPX_VOICE_WRITE(VoiceRelease, m_ahVoices[i]);
        }
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  WaitForVoiceOff
 *
 *  Description:
 *      Busy-waits for a voice that's in the process of being turned off to
 *      actually become inactive.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::WaitForVoiceOff"

void
CMcpxVoiceClient::WaitForVoiceOff
(
    void
)
{
    static BOOL             fExplained  = FALSE;
    
    DPF_ENTER();

    if(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED)
    {
        ASSERT(!(m_dwStatus & MCPX_VOICESTATUS_ACTIVE));

#ifdef DEBUG

        if(m_dwStatus & MCPX_VOICESTATUS_VOICEOFF)
        {
            if(!g_fDirectSoundDisableBusyWaitWarning)
            {
                DPF_WARNING("Busy-waiting for the voice to turn off");

                if(!fExplained)
                {
                    DPF_WARNING("This warning is being generated because you've called a method that requires");
                    DPF_WARNING("a hardware voice to be stopped.  Stopping a voice is an asynchronous");
                    DPF_WARNING("operation, so any function that requires the voice to be stopped must block");
                    DPF_WARNING("until it is.  To see an example of this, call IDirectSoundBuffer::Stop and");
                    DPF_WARNING("immediately follow it with a call to IDirectSoundBuffer::GetStatus.  Chances");
                    DPF_WARNING("are, you'll see that DSBSTATUS_PLAYING is still set.  For a list of methods");
                    DPF_WARNING("that can potentially block in this method, consult the documentation.  To");
                    DPF_WARNING("query the playing status of a buffer or stream, call the GetStatus method on");
                    DPF_WARNING("the object you wish to check.  To disable this warning, set the global");
                    DPF_WARNING("variable \"g_fDirectSoundDisableBusyWaitWarning\" to TRUE.");

                    fExplained = TRUE;
                }
            }

#endif // DEBUG
        
            while(m_dwStatus & MCPX_VOICESTATUS_VOICEOFF)
            {
                CheckStuckVoice();
            }

#ifdef DEBUG

        }

#endif // DEBUG

    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  CheckStuckVoice
 *
 *  Description:
 *      Checks the amount of time that has elapsed since since a stop request
 *      was sent to the hardware.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::CheckStuckVoice"

void
CMcpxVoiceClient::CheckStuckVoice
(
    void
)
{

#ifdef MCPX_DEBUG_STUCK_VOICES

    static const REFERENCE_TIME rtTimeout   = 5000i64 * 10000i64;  // 5000ms

#else // MCPX_DEBUG_STUCK_VOICES

    static const REFERENCE_TIME rtTimeout   = 500i64 * 10000i64;   // 500ms

#endif // MCPX_DEBUG_STUCK_VOICES

    REFERENCE_TIME              rtCurrent;
    
    DPF_ENTER();

    if((m_dwStatus & MCPX_VOICESTATUS_VOICEOFFMASK) == MCPX_VOICESTATUS_VOICEOFFMASK)
    {
        //
        // We're still waiting for the voice to turn off.  Check to see how
        // long it's been since we called VoiceOff.
        //
        
        ASSERT(m_rtVoiceOff > 0);
        ASSERT(!m_pMcpxApu->m_dwVoiceMapLock);
        
        KeQuerySystemTime((PLARGE_INTEGER)&rtCurrent);

        if(rtCurrent >= m_rtVoiceOff + rtTimeout)
        {
            DPF_WARNING("Voice %x (%x) appears to be stuck.  Forcing it to turn off...", this, m_ahVoices[0]);

            RemoveStuckVoice();
        }
    }
    else
    {
        //
        // The voice is turned off.  Make sure the CHECKSTUCK deferred
        // command is removed from the queue.
        //
        
        RemoveDeferredCommand(MCPX_DEFERREDCMD_VOICE_CHECKSTUCK);
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  PauseVoice
 *
 *  Description:
 *      Pauses or resumes a running voice.
 *
 *  Arguments:
 *      DWORD [in]: paused/starving status bitmask.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::PauseVoice"

void
CMcpxVoiceClient::PauseVoice
(
    DWORD                   dwStatus
)
{
    DWORD                   dwRegisterValue;
    DWORD                   i;
    
    DPF_ENTER();
    AutoIrql();

    dwStatus &= MCPX_VOICESTATUS_ALLPAUSEDMASK;

    //
    // Pause or resume the voice
    //

    if(MAKEBOOL(dwStatus) != MAKEBOOL(m_dwStatus & MCPX_VOICESTATUS_ALLPAUSEDMASK))
    {
        if((m_dwStatus & MCPX_VOICESTATUS_ACTIVEMASK) == MCPX_VOICESTATUS_ACTIVEMASK)
        {
            if(dwStatus)
            {
                dwRegisterValue = MCPX_MAKE_REG_VALUE(NV1BA0_PIO_VOICE_PAUSE_ACTION_STOP, NV1BA0_PIO_VOICE_PAUSE_ACTION);
            }
            else
            {
                dwRegisterValue = MCPX_MAKE_REG_VALUE(NV1BA0_PIO_VOICE_PAUSE_ACTION_RESUME, NV1BA0_PIO_VOICE_PAUSE_ACTION);
            }

            MCPX_CHECK_VOICE_FIFO(1 * m_bVoiceCount);
            
            for(i = 0; i < m_bVoiceCount; i++)
            {
                MCPX_VOICE_WRITE(VoicePause, MCPX_SET_REG_VALUE(dwRegisterValue, m_ahVoices[i], NV1BA0_PIO_VOICE_PAUSE_HANDLE));
            }
        }

        DPF_INFO("Voice %x (%x) pause state going from %x to %x", this, m_ahVoices[0], m_dwStatus & MCPX_VOICESTATUS_ALLPAUSEDMASK, dwStatus);
    }

    //
    // Update the status
    //

    and(&m_dwStatus, ~MCPX_VOICESTATUS_ALLPAUSEDMASK);
    or(&m_dwStatus, (WORD)dwStatus);

    DPF_LEAVE_VOID();
}


/***************************************************************************
 *
 *  Commit3dSettings
 *
 *  Description:
 *      Commits deferred data to the device.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      DWORD: parameter mask.
 *
 ***************************************************************************/

#ifndef MCPX_BOOT_LIB

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::Commit3dSettings"

DWORD
CMcpxVoiceClient::Commit3dSettings
(   
    void
)
{
    DWORD                   dwParameterMask     = 0;
    DWORD                   dw3dChangeMask;
    DWORD                   dwMixBinChangeMask;
    DWORD                   dwI3dl2ChangeMask;

    DPF_ENTER();
    AutoFpState();

    ASSERT(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D);
    ASSERT(m_pSettings->m_p3dParams);
    ASSERT(m_pHrtfSource);
    ASSERT(m_pI3dl2Source);

    //
    // If the voice isn't active, don't bother doing the recalc yet.  We'll
    // do it when the voice is activated.
    //
    
    if((m_dwStatus & MCPX_VOICESTATUS_ACTIVEMASK) == MCPX_VOICESTATUS_ACTIVEMASK)
    {
        //
        // If the mode changed at all, a position recalculation is required.
        // This is because any change from NORMAL to HEADRELATIVE requires
        // the position recalc, but since you could go from NORMAL to 
        // DISABLED to HEADRELATIVE, we'll just take the hit.
        //
        // If we went from ENABLED to DISABLED, we don't actually want to
        // recalculate anything yet, but we do have to reset everything 3D
        // affects.
        //
        // If not, we'll go ahead and recalculate and reset the parameter
        // mask.  Because changing the mode to DISABLED doesn't clear the
        // parameter mask, any remaining changes will be recalculated and
        // applied now.
        //

        dw3dChangeMask = 0;
        dwMixBinChangeMask = 0;
        dwI3dl2ChangeMask = 0;
        
        if(m_dw3dMode != m_pSettings->m_p3dParams->HrtfParams.dwMode)
        {
            if((DS3DMODE_DISABLE == m_pSettings->m_p3dParams->HrtfParams.dwMode) != (DS3DMODE_DISABLE == m_dw3dMode))
            {
                dw3dChangeMask |= MCPX_3DAPPLY_MASK;
                dwI3dl2ChangeMask |= MCPX_I3DL2APPLY_MASK;
                dwMixBinChangeMask |= ~0UL;
            }

            m_dw3dMode = m_pSettings->m_p3dParams->HrtfParams.dwMode;
            m_pSettings->m_p3dParams->dwParameterMask |= DS3DPARAM_BUFFER_POSITION;
        }
        
        //
        // Save a local copy of the parameter mask
        //
        
        dwParameterMask |= m_pSettings->m_p3dParams->dwParameterMask;

        //
        // Recalculate and check for changes that need to be applied
        //

        if(DS3DMODE_DISABLE != m_pSettings->m_p3dParams->HrtfParams.dwMode)
        {
            m_pHrtfSource->Calculate3d(dwParameterMask);

            dw3dChangeMask |= m_pHrtfSource->m_3dVoiceData.dwChangeMask;
            dwMixBinChangeMask |= m_pHrtfSource->m_3dVoiceData.dwMixBinChangeMask;

            if(dwParameterMask & DS3DPARAM_BUFFER_I3DL2)
            {
                m_pI3dl2Source->CalculateI3dl2(m_pHrtfSource->m_3dData.flMagPos);

                dwI3dl2ChangeMask |= m_pI3dl2Source->m_I3dl2Data.dwChangeMask;
            }
        }

        Apply3dSettings(dw3dChangeMask, dwMixBinChangeMask, dwI3dl2ChangeMask);
    }
    
    DPF_LEAVE(dwParameterMask);

    return dwParameterMask;
}

#endif // MCPX_BOOT_LIB


/***************************************************************************
 *
 *  Apply3dSettings
 *
 *  Description:
 *      Commits deferred data to the device.
 *
 *  Arguments:
 *      DWORD [in]: parameter mask.
 *
 *  Returns:  
 *      (void)
 *
 ***************************************************************************/

#ifndef MCPX_BOOT_LIB

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::Apply3dSettings"

void
CMcpxVoiceClient::Apply3dSettings
(
    DWORD                   dw3dChangeMask, 
    DWORD                   dwMixBinChangeMask, 
    DWORD                   dwI3dl2ChangeMask
)
{
    static const DWORD      dw3dVolumeMask      = MCPX_3DAPPLY_DISTANCEVOLUME | MCPX_3DAPPLY_CONEVOLUME | MCPX_3DAPPLY_FRONTVOLUME | MCPX_3DAPPLY_REARVOLUME;
    static const DWORD      dwI3dl2VolumeMask   = MCPX_I3DL2APPLY_DIRECT | MCPX_I3DL2APPLY_SOURCE;
    static const DWORD      dwPitchMask         = MCPX_3DAPPLY_DOPPLERPITCH;
    static const DWORD      dwHrtfMask          = MCPX_3DAPPLY_FILTERPAIR;
    static const DWORD      dwFilterMask        = MCPX_I3DL2APPLY_DIRECTIIR | MCPX_I3DL2APPLY_REVERBIIR;
    
    DPF_ENTER();

    if((dw3dChangeMask & dw3dVolumeMask) || (dwMixBinChangeMask) || (dwI3dl2ChangeMask & dwI3dl2VolumeMask))
    {
        SetVolume();
    }

    if(dw3dChangeMask & dwPitchMask)
    {
        SetPitch();
    }
    
    if(dw3dChangeMask & dwHrtfMask)
    {
        LoadHrtfFilter();
    }

    if(dwI3dl2ChangeMask & dwFilterMask)
    {
        SetFilter(NULL);
    }

    DPF_LEAVE_VOID();
}

#endif // MCPX_BOOT_LIB


/****************************************************************************
 *
 *  LoadHrtfFilter
 *
 *  Description:
 *      Loads the HRTF filter for the voice to use.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#ifndef MCPX_BOOT_LIB

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::LoadHrtfFilter"

void
CMcpxVoiceClient::LoadHrtfFilter
(
    void
)
{
    static const FIRFILTER8     DisabledFilterData  =
    {
        { 0x00, 0x55, 0x55, 0x9F, 0x22, 0x41, 0x06, 0xA4, 0x11, 0x27, 0xAA, 0xB2, 0x2d, 0x2d, 0x09, 0x0D, 0x92, 0x96, 0x13, 0x0B, 0x9B, 0x81, 0x0D, 0x91, 0x07, 0x16, 0x05, 0x05, 0x06, 0x06, 0x02 },  0    /*  left, #1170 */
    };

    static const HRTFFILTERPAIR DisabledFilterPair  =
    {
        &DisabledFilterData,
        &DisabledFilterData
    };

    LPCHRTFFILTERPAIR           pFilterPair;
    DWORD                       dwFilterIndex;
    DWORD                       hrir;
    DWORD                       hrirx;
    LONG                        itd;
    DWORD                       i;
    
    DPF_ENTER();
    AutoIrql();

    ASSERT(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED);
    ASSERT(m_pSettings->m_p3dParams);
    ASSERT(m_pHrtfSource);

    //
    // Get pointers to the HRTF coefficients
    //
    // If we're not 3D or the 3D mode is DISABLED, we'll disable HRTF by
    // loading the coefficients for 0,0 azimuth/elevation.
    //
    // If the HRTF coefficients ever change, the filter pair used here will
    // need to be updated.
    //

    if(DS3DMODE_DISABLE == m_pSettings->m_p3dParams->HrtfParams.dwMode)
    {
        pFilterPair = &DisabledFilterPair;
    }
    else if(!m_pHrtfSource->m_3dVoiceData.FilterPair.pLeftFilter)
    {
        pFilterPair = &DisabledFilterPair;
    }
    else if(!m_pHrtfSource->m_3dVoiceData.FilterPair.pRightFilter)
    {
        pFilterPair = &DisabledFilterPair;
    }
    else
    {
        pFilterPair = &m_pHrtfSource->m_3dVoiceData.FilterPair;
    }

    //
    // Alternate between the 3D filters.  Each 3D voice actually gets 
    // allocated 2 hardware 3D filters.  This way, we can update one of
    // them while the other one is still being processed.  This prevents
    // us from stomping on the filter coefficients while the hardware is
    // in the middle of processing a frame.
    //

    ASSERT(m_bAvailable3dFilter < MCPX_HW_3DFILTERS_PER_VOICE);

    dwFilterIndex = ((DWORD)m_ahVoices[0] * MCPX_HW_3DFILTERS_PER_VOICE) + m_bAvailable3dFilter;
    m_bAvailable3dFilter = !m_bAvailable3dFilter;

    //
    // Load the coefficients
    //

    MCPX_CHECK_VOICE_FIFO(1 + (NUMELMS(pFilterPair->pLeftFilter->Coeff) - 1) / 2 + 1 + 2);

    MCPX_VOICE_WRITE(SetCurrentHRTFEntry, MCPX_MAKE_REG_VALUE(dwFilterIndex, NV1BA0_PIO_SET_CURRENT_HRTF_ENTRY_HANDLE));

    for(i = 0; i < (NUMELMS(pFilterPair->pLeftFilter->Coeff) - 1) / 2; i++)
    {
        hrir = MCPX_MAKE_REG_VALUE(pFilterPair->pLeftFilter->Coeff[i * 2], NV1BA0_PIO_SET_HRIR_LEFT0);
        hrir |= MCPX_MAKE_REG_VALUE(pFilterPair->pRightFilter->Coeff[i * 2], NV1BA0_PIO_SET_HRIR_RIGHT0);
        hrir |= MCPX_MAKE_REG_VALUE(pFilterPair->pLeftFilter->Coeff[(i * 2) + 1], NV1BA0_PIO_SET_HRIR_LEFT1);
        hrir |= MCPX_MAKE_REG_VALUE(pFilterPair->pRightFilter->Coeff[(i * 2) + 1], NV1BA0_PIO_SET_HRIR_RIGHT1);

        MCPX_VOICE_WRITE(SetHRIR[i], hrir);
    }

    hrirx = MCPX_MAKE_REG_VALUE(pFilterPair->pLeftFilter->Coeff[i * 2], NV1BA0_PIO_SET_HRIR_X_LEFT30);
    hrirx |= MCPX_MAKE_REG_VALUE(pFilterPair->pRightFilter->Coeff[i * 2], NV1BA0_PIO_SET_HRIR_X_RIGHT30);

    //
    // Set filter delay
    //

    ASSERT(m_pHrtfSource);
    
    if(m_pHrtfSource->m_3dData.flAzimuth >= 0.0f)
    {
        itd = (LONG)pFilterPair->pLeftFilter->Delay;
    }
    else
    {
        itd = -(LONG)pFilterPair->pRightFilter->Delay;
    }

    ASSERT((itd >= -42) && (itd <= 42));

    itd <<= 9;

    itd = MCPX_MAKE_REG_VALUE(itd, NV1BA0_PIO_SET_HRIR_X_ITD);

    MCPX_VOICE_WRITE(SetHRIRX, hrirx | itd);

    //
    // Tell the voice to use the coefficient we just loaded
    //

    MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[0]);
    MCPX_VOICE_WRITE(SetVoiceTarHRTF, MCPX_MAKE_REG_VALUE(dwFilterIndex, NV1BA0_PIO_SET_VOICE_TAR_HRTF_HANDLE));

    DPF_LEAVE_VOID();
}

#endif // MCPX_BOOT_LIB


/****************************************************************************
 *
 *  SetLFO
 *
 *  Description:
 *      Sets parameters for one of the low-frequency occilator.
 *
 *  Arguments:
 *      LPCDSLFODESC [in]: common LFO settings.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::SetLFO"

HRESULT
CMcpxVoiceClient::SetLFO
(
    LPCDSLFODESC            pLfo
)
{
    DWORD                   i;
    
    DPF_ENTER();
    AutoIrql();

    //
    // Recalculate register values
    //
    
    if(DSLFO_MULTI == pLfo->dwLFO)
    {
        m_RegCache.CfgMISC = MCPX_SET_REG_VALUE(m_RegCache.CfgMISC, MAKEBOOL(pLfo->dwDelay), NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOA_DELAYMODE);
        
        m_RegCache.CfgLFODLY = MCPX_SET_REG_VALUE(m_RegCache.CfgLFODLY, pLfo->dwDelay, NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOADLY);
        
        m_RegCache.LFOENV = MCPX_SET_REG_VALUE(m_RegCache.LFOENV, pLfo->dwDelta, NV1BA0_PIO_SET_VOICE_LFO_ENV_LFOADLT);
        
        m_RegCache.LFOMOD = MCPX_SET_REG_VALUE(m_RegCache.LFOMOD, pLfo->lAmplitudeModulation, NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOAAM);
        m_RegCache.LFOMOD = MCPX_SET_REG_VALUE(m_RegCache.LFOMOD, pLfo->lPitchModulation, NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOAFM);
        m_RegCache.LFOMOD = MCPX_SET_REG_VALUE(m_RegCache.LFOMOD, pLfo->lFilterCutOffRange, NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOAFC);
    }
    else if(DSLFO_PITCH == pLfo->dwLFO)
    {
        m_RegCache.CfgMISC = MCPX_SET_REG_VALUE(m_RegCache.CfgMISC, MAKEBOOL(pLfo->dwDelay), NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOF_DELAYMODE);
        
        m_RegCache.CfgLFODLY = MCPX_SET_REG_VALUE(m_RegCache.CfgLFODLY, pLfo->dwDelay, NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOFDLY);
        
        m_RegCache.LFOENV = MCPX_SET_REG_VALUE(m_RegCache.LFOENV, pLfo->dwDelta, NV1BA0_PIO_SET_VOICE_LFO_ENV_LFOFDLT);
        
        m_RegCache.LFOMOD = MCPX_SET_REG_VALUE(m_RegCache.LFOMOD, pLfo->lPitchModulation, NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOFFM);
    }
    else
    {
        ASSERTMSG("Invalid LFO identifier");
    }

    //
    // Apply changes
    //
    
    if(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED)
    {
        MCPX_CHECK_VOICE_FIFO(7 * m_bVoiceCount);
        
        for(i = 0; i < m_bVoiceCount; i++)
        {
            MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);
            MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_ON);
            MCPX_VOICE_WRITE(SetVoiceCfgMISC, m_RegCache.CfgMISC);
            MCPX_VOICE_WRITE(SetVoiceCfgLFODLY, m_RegCache.CfgLFODLY);
            MCPX_VOICE_WRITE(SetVoiceLFOENV, m_RegCache.LFOENV);
            MCPX_VOICE_WRITE(SetVoiceLFOMOD, m_RegCache.LFOMOD);
            MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_OFF);
        }
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  SetEG
 *
 *  Description:
 *      Sets parameters for one of the envelope generators.
 *
 *  Arguments:
 *      LPCDSENVELOPEDESC [in]: common envelope settings.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::SetEG"

HRESULT
CMcpxVoiceClient::SetEG
(
    LPCDSENVELOPEDESC       pEnv
)
{
    DWORD                   i;
    
    DPF_ENTER();
    AutoIrql();

    //
    // Recalculate register values
    //
    
    if(DSEG_MULTI == pEnv->dwEG)
    {
        m_RegCache.VoiceOn = MCPX_SET_REG_VALUE(m_RegCache.VoiceOn, pEnv->dwMode, NV1BA0_PIO_VOICE_ON_ENVF);
        
        m_RegCache.CfgMISC = MCPX_SET_REG_VALUE(m_RegCache.CfgMISC, pEnv->dwRelease, NV1BA0_PIO_SET_VOICE_CFG_MISC_EF_RELEASERATE);
        
        m_RegCache.CfgENV0 = MCPX_SET_REG_VALUE(m_RegCache.CfgENV0, pEnv->lPitchScale, NV1BA0_PIO_SET_VOICE_CFG_ENV0_EF_PITCHSCALE);
        
        m_RegCache.CfgENV1 = MCPX_SET_REG_VALUE(m_RegCache.CfgENV1, pEnv->dwAttack, NV1BA0_PIO_SET_VOICE_CFG_ENV1_EF_ATTACKRATE);
        m_RegCache.CfgENV1 = MCPX_SET_REG_VALUE(m_RegCache.CfgENV1, pEnv->dwDelay, NV1BA0_PIO_SET_VOICE_CFG_ENV1_EF_DELAYTIME);
        m_RegCache.CfgENV1 = MCPX_SET_REG_VALUE(m_RegCache.CfgENV1, pEnv->lFilterCutOff, NV1BA0_PIO_SET_VOICE_CFG_ENV1_EF_FCSCALE);
        
        m_RegCache.CfgENVF = MCPX_SET_REG_VALUE(m_RegCache.CfgENVF, pEnv->dwDecay, NV1BA0_PIO_SET_VOICE_CFG_ENVF_EF_DECAYRATE);
        m_RegCache.CfgENVF = MCPX_SET_REG_VALUE(m_RegCache.CfgENVF, pEnv->dwHold, NV1BA0_PIO_SET_VOICE_CFG_ENVF_EF_HOLDTIME);
        m_RegCache.CfgENVF = MCPX_SET_REG_VALUE(m_RegCache.CfgENVF, pEnv->dwSustain, NV1BA0_PIO_SET_VOICE_CFG_ENVF_EF_SUSTAINLEVEL);
    }
    else if(DSEG_AMPLITUDE == pEnv->dwEG)
    {
        m_RegCache.VoiceOn = MCPX_SET_REG_VALUE(m_RegCache.VoiceOn, pEnv->dwMode, NV1BA0_PIO_VOICE_ON_ENVA);

        m_RegCache.LFOENV = MCPX_SET_REG_VALUE(m_RegCache.LFOENV, pEnv->dwRelease, NV1BA0_PIO_SET_VOICE_LFO_ENV_EA_RELEASERATE);

        m_RegCache.CfgENV0 = MCPX_SET_REG_VALUE(m_RegCache.CfgENV0, pEnv->dwAttack, NV1BA0_PIO_SET_VOICE_CFG_ENV0_EA_ATTACKRATE);
        m_RegCache.CfgENV0 = MCPX_SET_REG_VALUE(m_RegCache.CfgENV0, pEnv->dwDelay, NV1BA0_PIO_SET_VOICE_CFG_ENV0_EA_DELAYTIME);

        m_RegCache.CfgENVA = MCPX_SET_REG_VALUE(m_RegCache.CfgENVA, pEnv->dwDecay, NV1BA0_PIO_SET_VOICE_CFG_ENVA_EA_DECAYRATE);
        m_RegCache.CfgENVA = MCPX_SET_REG_VALUE(m_RegCache.CfgENVA, pEnv->dwHold, NV1BA0_PIO_SET_VOICE_CFG_ENVA_EA_HOLDTIME);
        m_RegCache.CfgENVA = MCPX_SET_REG_VALUE(m_RegCache.CfgENVA, pEnv->dwSustain, NV1BA0_PIO_SET_VOICE_CFG_ENVA_EA_SUSTAINLEVEL);
    }
    else
    {
        ASSERTMSG("Invalid EG identifier");
    }

    //
    // Apply changes
    //
    
    if(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED)
    {
        MCPX_CHECK_VOICE_FIFO(9 * m_bVoiceCount);
        
        for(i = 0; i < m_bVoiceCount; i++)
        {
            MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);
            MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_ON);
            MCPX_VOICE_WRITE(SetVoiceCfgMISC, m_RegCache.CfgMISC);
            MCPX_VOICE_WRITE(SetVoiceCfgENV0, m_RegCache.CfgENV0);
            MCPX_VOICE_WRITE(SetVoiceCfgENVA, m_RegCache.CfgENVA);
            MCPX_VOICE_WRITE(SetVoiceCfgENV1, m_RegCache.CfgENV1);
            MCPX_VOICE_WRITE(SetVoiceCfgENVF, m_RegCache.CfgENVF);
            MCPX_VOICE_WRITE(SetVoiceLFOENV, m_RegCache.LFOENV);
            MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_OFF);
        }
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  SetFilter
 *
 *  Description:
 *      Sets parameters for the filter.
 *
 *  Arguments:
 *      LPCDSFILTERDESC [in]: filter settings.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::SetFilter"

HRESULT
CMcpxVoiceClient::SetFilter
(
    LPCDSFILTERDESC         pFilter
)
{
    DSFILTERDESC            Filter;
    DWORD                   i;
    
    DPF_ENTER();
    AutoIrql();

    //
    // Make a local copy of the filter data or create one based on current
    // register values.
    //

    if(pFilter)
    {
        CopyMemory(&Filter, pFilter, sizeof(*pFilter));
    }
    else
    {
        Filter.dwMode = MCPX_GET_REG_VALUE(m_RegCache.CfgMISC, NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE);
        Filter.dwQCoefficient = MCPX_GET_REG_VALUE(m_RegCache.CfgMISC, NV1BA0_PIO_SET_VOICE_CFG_MISC_BPQ);

        Filter.adwCoefficients[0] = MCPX_GET_REG_VALUE(m_RegCache.TarFCA, NV1BA0_PIO_SET_VOICE_TAR_FCA_FC0);
        Filter.adwCoefficients[1] = MCPX_GET_REG_VALUE(m_RegCache.TarFCA, NV1BA0_PIO_SET_VOICE_TAR_FCA_FC1);

        Filter.adwCoefficients[2] = MCPX_GET_REG_VALUE(m_RegCache.TarFCB, NV1BA0_PIO_SET_VOICE_TAR_FCB_FC2);
        Filter.adwCoefficients[3] = MCPX_GET_REG_VALUE(m_RegCache.TarFCB, NV1BA0_PIO_SET_VOICE_TAR_FCB_FC3);
    }

#ifndef MCPX_BOOT_LIB

    //
    // If we're 3D, hack the filter mode so I3DL2 is always turned on
    //

    if(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D)
    {
        if(DSFILTER_MODE_BYPASS == Filter.dwMode)
        {
            Filter.dwMode = DSFILTER_MODE_MULTI;
        }
    }

#endif // MCPX_BOOT_LIB

    //
    // Recalculate register values
    //
    
    m_RegCache.CfgMISC = MCPX_SET_REG_VALUE(m_RegCache.CfgMISC, Filter.dwMode, NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE);
    m_RegCache.CfgMISC = MCPX_SET_REG_VALUE(m_RegCache.CfgMISC, Filter.dwQCoefficient, NV1BA0_PIO_SET_VOICE_CFG_MISC_BPQ);

    m_RegCache.TarFCA = MCPX_MAKE_REG_VALUE(Filter.adwCoefficients[0], NV1BA0_PIO_SET_VOICE_TAR_FCA_FC0);
    m_RegCache.TarFCA |= MCPX_MAKE_REG_VALUE(Filter.adwCoefficients[1], NV1BA0_PIO_SET_VOICE_TAR_FCA_FC1);

#ifndef MCPX_BOOT_LIB

    if(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D)
    {
        ASSERT(m_pSettings->m_p3dParams);
        ASSERT(m_pI3dl2Source);
        
        if(DS3DMODE_DISABLE == m_pSettings->m_p3dParams->HrtfParams.dwMode)
        {
            m_RegCache.TarFCB = 0;
        }
        else            
        {
            m_RegCache.TarFCB = MCPX_MAKE_REG_VALUE(m_pI3dl2Source->m_I3dl2Data.nDirectIir, NV_PAVS_VOICE_CUR_FCB_FC2);
            m_RegCache.TarFCB |= MCPX_MAKE_REG_VALUE(m_pI3dl2Source->m_I3dl2Data.nReverbIir, NV_PAVS_VOICE_CUR_FCB_FC3);
        }
    }
    else

#endif // MCPX_BOOT_LIB

    {
        m_RegCache.TarFCB = MCPX_MAKE_REG_VALUE(Filter.adwCoefficients[2], NV_PAVS_VOICE_CUR_FCB_FC2);
        m_RegCache.TarFCB |= MCPX_MAKE_REG_VALUE(Filter.adwCoefficients[3], NV_PAVS_VOICE_CUR_FCB_FC3);
    }

    //
    // Apply changes
    //
    
    if(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED)
    {
        MCPX_CHECK_VOICE_FIFO(6 * m_bVoiceCount);
        
        for(i = 0; i < m_bVoiceCount; i++)
        {
            MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);
            MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_ON);
            MCPX_VOICE_WRITE(SetVoiceCfgMISC, m_RegCache.CfgMISC);
            MCPX_VOICE_WRITE(SetVoiceTarFCA, m_RegCache.TarFCA);
            MCPX_VOICE_WRITE(SetVoiceTarFCB, m_RegCache.TarFCB);
            MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_OFF);
        }
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  ConnectVoice
 *
 *  Description:
 *      Sets the submix destination for this voice.
 *
 *  Arguments:
 *      CMcpxVoiceClient * [in]: destination voice.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::ConnectVoice"

HRESULT
CMcpxVoiceClient::ConnectVoice
(
    void
)
{
    CMcpxBuffer *           pSubMixDestination;
    DWORD                   dwVoiceBins;
    MCPX_VOICE_VOLUME       Volume;
    DWORD                   dwPitch;
    DWORD                   i;
    
    DPF_ENTER();
    AutoIrql();

    pSubMixDestination = GetSubMixDestination();
    ASSERT(pSubMixDestination);

    DPF_INFO("Voice %x (%x) using %x (%x) as it's submix destination", this, m_ahVoices[0], pSubMixDestination, pSubMixDestination->m_ahVoices[0]);

    //
    // Make sure we're not active.  The voice has to be turned off in order
    // to change it's position in the voice list.
    //
    
    if((m_dwStatus & MCPX_VOICESTATUS_ACTIVEMASK) == MCPX_VOICESTATUS_ACTIVEMASK)
    {
        DPF_WARNING("Voice %x (%x) is still active.  Shutting it down...", this, m_ahVoices[0]);
        DeactivateVoice();
    }

    //
    // Add us to the destination voice's list
    //

    InsertTailList(&pSubMixDestination->m_lstSourceVoices, &m_leSourceVoice);

    //
    // Update our mixbin assignments, volume and pitch
    //

    if(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED)
    {
        ConvertMixBinValues(&dwVoiceBins, &m_RegCache.CfgFMT);
        ConvertVolumeValues(&Volume);
        ConvertPitchValue(&dwPitch);

        MCPX_CHECK_VOICE_FIFO(9 * m_bVoiceCount);
        
        for(i = 0; i < m_bVoiceCount; i++)
        {
            MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);
            MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_ON);
            MCPX_VOICE_WRITE(SetVoiceCfgVBIN, dwVoiceBins);
            MCPX_VOICE_WRITE(SetVoiceCfgFMT, GetVoiceCfgFMT(i));
            MCPX_VOICE_WRITE(SetVoiceTarVOLA, Volume.TarVOLA[i]);
            MCPX_VOICE_WRITE(SetVoiceTarVOLB, Volume.TarVOLB[i]);
            MCPX_VOICE_WRITE(SetVoiceTarVOLC, Volume.TarVOLC[i]);
            MCPX_VOICE_WRITE(SetVoiceTarPitch, dwPitch);
            MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_OFF);
        }
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  DisconnectVoice
 *
 *  Description:
 *      Disconnects this voice from a submix chain.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::DisconnectVoice"

HRESULT
CMcpxVoiceClient::DisconnectVoice
(
    void
)
{
    CMcpxBuffer *           pSubMixDestination;
    DWORD                   dwVoiceBins;
    MCPX_VOICE_VOLUME       Volume;
    DWORD                   dwPitch;
    HRESULT                 hr;
    DWORD                   i;
    
    DPF_ENTER();
    AutoIrql();

    if(pSubMixDestination = GetSubMixDestination())
    {
        //
        // Remove ourselves from the destination voices's list
        //

        RemoveEntryList(&m_leSourceVoice);

        DPF_INFO("Voice %x (%x) disconnected submix from %x (%x)", this, m_ahVoices[0], pSubMixDestination, pSubMixDestination->m_ahVoices[0]);

        //
        // Update our mixbin assignments, volume and pitch
        //

        if(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED)
        {
            ConvertMixBinValues(&dwVoiceBins, &m_RegCache.CfgFMT);
            ConvertVolumeValues(&Volume);
            ConvertPitchValue(&dwPitch);

            MCPX_CHECK_VOICE_FIFO(9 * m_bVoiceCount);
            
            for(i = 0; i < m_bVoiceCount; i++)
            {
                MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);
                MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_ON);
                MCPX_VOICE_WRITE(SetVoiceCfgVBIN, dwVoiceBins);
                MCPX_VOICE_WRITE(SetVoiceCfgFMT, GetVoiceCfgFMT(i));
                MCPX_VOICE_WRITE(SetVoiceTarVOLA, Volume.TarVOLA[i]);
                MCPX_VOICE_WRITE(SetVoiceTarVOLB, Volume.TarVOLB[i]);
                MCPX_VOICE_WRITE(SetVoiceTarVOLC, Volume.TarVOLC[i]);
                MCPX_VOICE_WRITE(SetVoiceTarPitch, dwPitch);
                MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_OFF);
            }
        }
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  GetSslPosition
 *
 *  Description:
 *      Gets the current SSL play cursor position.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      DWORD: cursor position, in bytes.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxVoiceClient::GetSslPosition"

DWORD
CMcpxVoiceClient::GetSslPosition
(
    void
)
{
    DWORD                   dwPlayCursor    = 0;
    DWORD                   dwState;
    
    DPF_ENTER();
    AutoIrql();

    //
    // There's two ways to query the hardware for current position.  We could 
    // use the "recommended" way and poke a position request register, wait for 
    // an interrupt, then get the current position out of a notifier.  Or, we
    // could go the simple route and just look at the voice structure.  NVidia
    // doesn't like this solution because they want to be able to change the
    // voice structure without modifying any driver code.  Our part won't change,
    // and the driver could be modified either way.  Hm... let's see... I think
    // I prefer the lesser amount of code and no interrupts.
    //
    // We're locking the voice so we don't query the voice structure while the
    // APU is writing to it.  It's possible we could read partially written
    // data.
    // 

    if(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED)
    {
        //
        // Check for the NEW_VOICE bit in the voice state.  The hardware 
        // doesn't update the position register until the first frame has 
        // been processed.
        //

        MCPX_VOICE_STRUCT_READ(m_ahVoices[m_bVoiceCount - 1], NV_PAVS_VOICE_PAR_STATE, &dwState);

        if(!MCPX_GET_REG_VALUE(dwState, NV_PAVS_VOICE_PAR_STATE_NEW_VOICE))
        {
            MCPX_CHECK_VOICE_FIFO(3);
        
            MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[0]);
            MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_ON);
            MCPX_VOICE_STRUCT_READ(m_ahVoices[0], NV_PAVS_VOICE_PAR_OFFSET, &dwPlayCursor);
            MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_OFF);

            dwPlayCursor = MCPX_GET_REG_VALUE(dwPlayCursor, NV_PAVS_VOICE_PAR_OFFSET_CBO);
            dwPlayCursor = SamplesToBytes(dwPlayCursor);
        }
    }

    DPF_LEAVE(dwPlayCursor);

    return dwPlayCursor;
}


