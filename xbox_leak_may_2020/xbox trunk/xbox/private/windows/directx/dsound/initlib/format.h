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
            (DWORD)(frequency) / 64 * 36, \
            (WORD)(channels) * 36, \
            4, \
            2 \
        }, \
        64 \
    }

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
            (DWORD)(frequency) / 64 * 36, \
            (WORD)(channels) * 36, \
            4, \
            2 \
        }, \
        64 \
        (DWORD)(channelmask), \
        KSDATAFORMAT_SUBTYPE_XBOX_ADPCM \
    }

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

EXTERN_C DWORD FmtCreateInternal(LPDSWAVEFORMAT pDest, LPCWAVEFORMATEX pSource);
EXTERN_C BOOL FmtIsValid(LPCWAVEFORMATEX pwfx);
EXTERN_C void FmtPcmToInternal(LPDSWAVEFORMAT pDest, LPCWAVEFORMATEX pSource);
EXTERN_C BOOL FmtIsValidPcm(LPCWAVEFORMATEX pwfx);
EXTERN_C void FmtAdpcmToInternal(LPDSWAVEFORMAT pDest, LPCXBOXADPCMWAVEFORMAT pSource);
EXTERN_C BOOL FmtIsValidAdpcm(LPCXBOXADPCMWAVEFORMAT pwfx);

#ifndef MCPX_BOOT_LIB

EXTERN_C DWORD FmtExtensibleToInternal(LPDSWAVEFORMAT pDest, LPCWAVEFORMATEXTENSIBLE pSource);
EXTERN_C BOOL FmtIsValidExtensible(LPCWAVEFORMATEXTENSIBLE pwfx);

#endif // MCPX_BOOT_LIB

#endif // __FORMAT_H__
