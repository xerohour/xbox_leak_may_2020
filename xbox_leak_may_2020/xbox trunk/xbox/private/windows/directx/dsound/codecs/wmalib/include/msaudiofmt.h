//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       msaudiofmt.h
//
//--------------------------------------------------------------------------

/* msaudiofmt.h */

#ifndef _MSAUDIO_FMT_H
#define _MSAUDIO_FMT_H

//NQF+, 120199
#ifndef macintosh
#include "../common/macros.h"
#else
#include "macros.h"
#endif
//NQF-

#ifndef RC_INVOKED              // from mmreg.h
#pragma COMPILER_PACKALIGN(1)	// Assume byte packing throughout
#endif  // RC_INVOKED

// ======================================================================
// Windows Media Audio (common)


#define MM_MSFT_ACM_WMAUDIO  39

#define WMAUDIO_BITS_PER_SAMPLE    16 // just an uncompressed size...
#define WMAUDIO_MAX_CHANNELS       2


// ======================================================================
// Windows Media Audio V1 (a.k.a. "MSAudio")

#define WAVE_FORMAT_MSAUDIO1  0x0160
#define MM_MSFT_ACM_MSAUDIO1  39

typedef struct msaudio1waveformat_tag {
    WAVEFORMATEX wfx;
    U16          wSamplesPerBlock; // only counting "new" samples "= half of what will be used due to overlapping
    U16          wEncodeOptions;
} MSAUDIO1WAVEFORMAT;

typedef MSAUDIO1WAVEFORMAT *LPMSAUDIO1WAVEFORMAT;

#define MSAUDIO1_BITS_PER_SAMPLE    WMAUDIO_BITS_PER_SAMPLE
#define MSAUDIO1_MAX_CHANNELS       WMAUDIO_MAX_CHANNELS
#define MSAUDIO1_WFX_EXTRA_BYTES    (sizeof(MSAUDIO1WAVEFORMAT) - sizeof(WAVEFORMATEX))


// ======================================================================
// Windows Media Audio V2

#define WAVE_FORMAT_WMAUDIO2  0x0161
#define MM_MSFT_ACM_WMAUDIO2  101

typedef struct wmaudio2waveformat_tag {
    WAVEFORMATEX wfx;
    U32          dwSamplesPerBlock; // only counting "new" samples "= half of what will be used due to overlapping
    U16          wEncodeOptions;
    U32          dwSuperBlockAlign; // the big size...  should be multiples of wfx.nBlockAlign.
} WMAUDIO2WAVEFORMAT;

typedef WMAUDIO2WAVEFORMAT *LPWMAUDIO2WAVEFORMAT;

#define WMAUDIO2_BITS_PER_SAMPLE    WMAUDIO_BITS_PER_SAMPLE
#define WMAUDIO2_MAX_CHANNELS       WMAUDIO_MAX_CHANNELS
#define WMAUDIO2_WFX_EXTRA_BYTES    (sizeof(WMAUDIO2WAVEFORMAT) - sizeof(WAVEFORMATEX))

#ifndef RC_INVOKED              // from mmreg.h
#pragma COMPILER_PACKALIGN_DEFAULT	// Revert to default packing
#endif  // RC_INVOKED

#endif /* !_MSAUDIO_FMT_H */
