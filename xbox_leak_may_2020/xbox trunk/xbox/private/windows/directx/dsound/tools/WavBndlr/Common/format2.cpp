/***************************************************************************
 *
 *  Copyright (C) 11/27/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       format2.cpp
 *  Content:    Format conversion helpers.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/27/2001   dereks  Created.
 *
 ****************************************************************************/

#include "wavbndli.h"

using namespace DirectSound::WaveFormat;


/****************************************************************************
 *
 *  WaveBankExpandFormat
 *
 *  Description:
 *      Expands a compressed wave format to a standard format structure.
 *
 *  Arguments:
 *      LPCWAVEBANKMINIWAVEFORMAT [in]: compressed format.
 *      LPWAVEBANKUNIWAVEFORMAT [out]: standard format.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "WaveBankExpandFormat"

BOOL 
WaveBankExpandFormat
(
    LPCWAVEBANKMINIWAVEFORMAT  pwfxCompressed, 
    LPWAVEBANKUNIWAVEFORMAT    pwfxExpanded
)
{
    DPF_ENTER();

    if(WAVEBANKMINIFORMAT_TAG_ADPCM == pwfxCompressed->wFormatTag)
    {
        XAudioCreateAdpcmFormat((WORD)pwfxCompressed->nChannels, pwfxCompressed->nSamplesPerSec, &pwfxExpanded->AdpcmWaveFormat);
    }
    else
    {
        XAudioCreatePcmFormat((WORD)pwfxCompressed->nChannels, pwfxCompressed->nSamplesPerSec, (WAVEBANKMINIFORMAT_BITDEPTH_16 == pwfxCompressed->wBitsPerSample) ? 16 : 8, &pwfxExpanded->WaveFormatEx);
    }

    DPF_LEAVE(TRUE);

    return TRUE;
}


/****************************************************************************
 *
 *  WaveBankCompressFormat
 *
 *  Description:
 *      Converts a standard format structure to a compressed wave format.
 *
 *  Arguments:
 *      LPCWAVEBANKUNIWAVEFORMAT [in]: standard format.
 *      LPWAVEBANKMINIWAVEFORMAT [out]: compressed format.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "WaveBankCompressFormat"

BOOL 
WaveBankCompressFormat
(
    LPCWAVEBANKUNIWAVEFORMAT   pwfxExpanded,
    LPWAVEBANKMINIWAVEFORMAT   pwfxCompressed
)
{
    BOOL                        fValid;

    DPF_ENTER();

    if(WAVE_FORMAT_PCM == pwfxExpanded->WaveFormatEx.wFormatTag)
    {
        if(fValid = IsValidPcmFormat(&pwfxExpanded->WaveFormatEx))
        {
            pwfxCompressed->wFormatTag = WAVEBANKMINIFORMAT_TAG_PCM;
            pwfxCompressed->nChannels = pwfxExpanded->WaveFormatEx.nChannels;
            pwfxCompressed->nSamplesPerSec = pwfxExpanded->WaveFormatEx.nSamplesPerSec;
            pwfxCompressed->wBitsPerSample = (16 == pwfxExpanded->WaveFormatEx.wBitsPerSample) ? WAVEBANKMINIFORMAT_BITDEPTH_16 : WAVEBANKMINIFORMAT_BITDEPTH_8;
        }
    }
    else if(WAVE_FORMAT_XBOX_ADPCM == pwfxExpanded->WaveFormatEx.wFormatTag)
    {
        if(fValid = IsValidAdpcmFormat(&pwfxExpanded->AdpcmWaveFormat))
        {
            pwfxCompressed->wFormatTag = WAVEBANKMINIFORMAT_TAG_ADPCM;
            pwfxCompressed->nChannels = pwfxExpanded->WaveFormatEx.nChannels;
            pwfxCompressed->nSamplesPerSec = pwfxExpanded->WaveFormatEx.nSamplesPerSec;
            pwfxCompressed->wBitsPerSample = WAVEBANKMINIFORMAT_BITDEPTH_4;
        }
    }
    else
    {
        DPF_ERROR("Unsupported format tag");
        fValid = FALSE;
    }   

    DPF_LEAVE(fValid);

    return fValid;
}


