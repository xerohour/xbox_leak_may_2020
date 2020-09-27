/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       format.h
 *  Content:    Wave format helper functions.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  10/10/00    dereks  Created.
 *
 ****************************************************************************/

#ifndef __FORMAT_H__
#define __FORMAT_H__

#ifdef __cplusplus

#include "macros.h"

//
// Xbox ADPCM definitions
//

#ifndef _XBOX

#define WAVE_FORMAT_XBOX_ADPCM 0x0069

DEFINETYPE(XBOXADPCMWAVEFORMAT, IMAADPCMWAVEFORMAT);

#endif // _XBOX

#define XBOX_ADPCM_SAMPLES_PER_BLOCK    64  // 64 samples per block
#define XBOX_ADPCM_BITS_PER_SAMPLE      4   // 4 bits per sample
#define XBOX_ADPCM_ENCODED_BLOCK_SIZE   36  // 64 mono PCM samples == 36 encoded bytes (72 for stereo)

//
// Format initialization macros
//

#define INIT_PCM_WAVEFORMAT_EX(channels, frequency, samplebits, containerbits) \
    { \
        WAVE_FORMAT_PCM, \
        (WORD)(channels), \
        (DWORD)(frequency), \
        (DWORD)(frequency) * (DWORD)(channels) * (DWORD)(containerbits) / 8, \
        (WORD)(channels) * (WORD)(containerbits) / 8, \
        (WORD)(samplebits), \
        0 \
    }

#define INIT_PCM_WAVEFORMAT(channels, frequency, samplebits) \
    INIT_PCM_WAVEFORMAT_EX(channels, frequency, samplebits, samplebits)

#define INIT_XBOX_ADPCM_WAVEFORMAT(channels, frequency) \
    { \
        { \
            WAVE_FORMAT_XBOX_ADPCM, \
            (WORD)(channels), \
            (DWORD)(frequency), \
            (DWORD)(frequency) / XBOX_ADPCM_SAMPLES_PER_BLOCK * XBOX_ADPCM_ENCODED_BLOCK_SIZE, \
            (WORD)(channels) * XBOX_ADPCM_ENCODED_BLOCK_SIZE, \
            XBOX_ADPCM_BITS_PER_SAMPLE, \
            2 \
        }, \
        XBOX_ADPCM_SAMPLES_PER_BLOCK \
    }

#ifdef _XBOX

#define INIT_PCM_WAVEFORMATEXTENSIBLE(channels, frequency, samplebits, containerbits, channelmask) \
    { \
        { \
            WAVE_FORMAT_EXTENSIBLE, \
            (WORD)(channels), \
            (DWORD)(frequency), \
            (DWORD)(frequency) * (DWORD)(channels) * (DWORD)(containerbits) / 8, \
            (WORD)(channels) * (WORD)(containerbits) / 8, \
            (WORD)(samplebits), \
            0 \
        }, \
        (WORD)(containerbits), \
        (DWORD)(channelmask), \
        KSDATAFORMAT_SUBTYPE_PCM \
    }
        
#define INIT_XBOX_ADPCM_WAVEFORMATEXTENSIBLE(channels, frequency, channelmask) \
    { \
        { \
            WAVE_FORMAT_EXTENSIBLE, \
            (WORD)(channels), \
            (DWORD)(frequency), \
            (DWORD)(frequency) / XBOX_ADPCM_SAMPLES_PER_BLOCK * XBOX_ADPCM_ENCODED_BLOCK_SIZE, \
            (WORD)(channels) * XBOX_ADPCM_ENCODED_BLOCK_SIZE, \
            XBOX_ADPCM_BITS_PER_SAMPLE, \
            2 \
        }, \
        XBOX_ADPCM_SAMPLES_PER_BLOCK \
        (DWORD)(channelmask), \
        KSDATAFORMAT_SUBTYPE_XBOX_ADPCM \
    }

#endif // _XBOX

//
// DirectSound internal format structure
//

BEGIN_DEFINE_STRUCT()
    WORD    wFormatTag;             // Format tag
    BYTE    nChannels;              // Channel count
    BYTE    wBitsPerSample;         // Count of bits per mono sample
    DWORD   nSamplesPerSec;         // Sampling rate
    DWORD   nBlockAlign;            // Block size of data (32-bit for alignment only)
END_DEFINE_STRUCT(DSWAVEFORMAT);

//
// Wave format helper functions
//

namespace DirectSound
{
    namespace WaveFormat
    {
        void CreatePcmFormat(WORD nChannels, DWORD nSamplesPerSec, WORD wBitsPerSample, LPWAVEFORMATEX pwfx);
        void CreateXboxAdpcmFormat(WORD nChannels, DWORD nSamplesPerSec, LPXBOXADPCMWAVEFORMAT pwfx);
        DWORD CreateInternalFormat(LPDSWAVEFORMAT pDest, LPCWAVEFORMATEX pSource);
        BOOL IsValidFormat(LPCWAVEFORMATEX pwfx);
        void PcmToInternalFormat(LPDSWAVEFORMAT pDest, LPCWAVEFORMATEX pSource);
        BOOL IsValidPcmFormat(LPCWAVEFORMATEX pwfx);
        void AdpcmToInternalFormat(LPDSWAVEFORMAT pDest, LPCXBOXADPCMWAVEFORMAT pSource);
        BOOL IsValidAdpcmFormat(LPCXBOXADPCMWAVEFORMAT pwfx);

#if defined(_XBOX) && !defined(MCPX_BOOT_LIB)

        DWORD ExtensibleToInternalFormat(LPDSWAVEFORMAT pDest, LPCWAVEFORMATEXTENSIBLE pSource);
        BOOL IsValidExtensibleFormat(LPCWAVEFORMATEXTENSIBLE pwfx);

#endif // defined(_XBOX) && !defined(MCPX_BOOT_LIB)
    
    };
}

#ifndef _XBOX

#define XAudioCreatePcmFormat DirectSound::WaveFormat::CreatePcmFormat
#define XAudioCreateAdpcmFormat DirectSound::WaveFormat::CreateXboxAdpcmFormat

#endif // _XBOX

using namespace DirectSound::WaveFormat;

#endif // __cplusplus

#endif // __FORMAT_H__
