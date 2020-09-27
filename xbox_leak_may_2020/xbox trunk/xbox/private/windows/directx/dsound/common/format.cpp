/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       format.cpp
 *  Content:    WAVEFORMATEX helper functions.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  10/10/00    dereks  Created.
 *
 ****************************************************************************/

#include "dscommon.h"

#ifndef _XBOX

#define DSBFREQUENCY_MIN 1
#define DSBFREQUENCY_MAX 0x7FFFFFFF

#endif // _XBOX


/****************************************************************************
 *
 *  CreatePcmFormat
 *
 *  Description:
 *      Creates a PCM wave format structure.
 *
 *  Arguments:
 *      WORD [in]: channel count.
 *      DWORD [in]: sampling rate.
 *      WORD [in]: count of bits per sample.
 *      LPWAVEFORMATEX [out]: format data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "WaveFormat::CreatePcmFormat"

void 
WaveFormat::CreatePcmFormat
(
    WORD                    nChannels, 
    DWORD                   nSamplesPerSec, 
    WORD                    wBitsPerSample, 
    LPWAVEFORMATEX          pwfx
)
{
    DPF_ENTER();
    
    ASSERT(pwfx);

    pwfx->wFormatTag = WAVE_FORMAT_PCM;
    pwfx->nChannels = nChannels;
    pwfx->nSamplesPerSec = nSamplesPerSec;
    pwfx->wBitsPerSample = wBitsPerSample;
    pwfx->nBlockAlign = nChannels * wBitsPerSample / 8;
    pwfx->nAvgBytesPerSec = nSamplesPerSec * pwfx->nBlockAlign;
    pwfx->cbSize = 0;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  CreateXboxAdpcmFormat
 *
 *  Description:
 *      Creates an Xbox ADPCM wave format structure.
 *
 *  Arguments:
 *      WORD [in]: channel count.
 *      DWORD [in]: sampling rate.
 *      LPWAVEFORMATEX [out]: format data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "WaveFormat::CreateXboxAdpcmFormat"

void 
WaveFormat::CreateXboxAdpcmFormat
(
    WORD                    nChannels, 
    DWORD                   nSamplesPerSec, 
    LPXBOXADPCMWAVEFORMAT   pwfx
)
{
    DPF_ENTER();
    
    ASSERT(pwfx);

    pwfx->wfx.wFormatTag = WAVE_FORMAT_XBOX_ADPCM;
    pwfx->wfx.nChannels = nChannels;
    pwfx->wfx.nSamplesPerSec = nSamplesPerSec;
    pwfx->wfx.wBitsPerSample = XBOX_ADPCM_BITS_PER_SAMPLE;
    pwfx->wfx.nBlockAlign = nChannels * XBOX_ADPCM_ENCODED_BLOCK_SIZE;
    pwfx->wfx.nAvgBytesPerSec = nSamplesPerSec / XBOX_ADPCM_SAMPLES_PER_BLOCK * XBOX_ADPCM_ENCODED_BLOCK_SIZE;
    pwfx->wfx.cbSize = sizeof(*pwfx) - sizeof(pwfx->wfx);
    pwfx->wSamplesPerBlock = XBOX_ADPCM_SAMPLES_PER_BLOCK;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  CreateInternalFormat
 *
 *  Description:
 *      Initializes a DSWAVEFORMAT structure based on a PCMWAVEFORMAT,
 *      WAVEFORMATEX or WAVEFORMATEX derivitave.
 *
 *  Arguments:
 *      LPDSWAVEFORMAT [out]: DirectSound format structure.
 *      LPCWAVEFORMATEX [in]: generic format structure.
 *
 *  Returns:  
 *      DWORD: mixbin assignment mask, if any.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "WaveFormat::CreateInternalFormat"

DWORD
WaveFormat::CreateInternalFormat
(
    LPDSWAVEFORMAT          pDest,
    LPCWAVEFORMATEX         pSource
)
{
    DWORD                   dwChannelMask   = 0;
    
    DPF_ENTER();

    ASSERT(pDest);
    ASSERT(pSource);

    switch(pSource->wFormatTag)
    {
        case WAVE_FORMAT_PCM:
            PcmToInternalFormat(pDest, pSource);
            break;

        case WAVE_FORMAT_XBOX_ADPCM:
            AdpcmToInternalFormat(pDest, (LPCXBOXADPCMWAVEFORMAT)pSource);
            break;

#if defined(_XBOX) && !defined(MCPX_BOOT_LIB)

        case WAVE_FORMAT_EXTENSIBLE:
            dwChannelMask = ExtensibleToInternalFormat(pDest, (LPCWAVEFORMATEXTENSIBLE)pSource);
            break;

#endif // defined(_XBOX) && !defined(MCPX_BOOT_LIB)

        default:
            ASSERTMSG("Unexpected format tag");
            break;
    }

    DPF_LEAVE(dwChannelMask);

    return dwChannelMask;
}
            

/****************************************************************************
 *
 *  IsValidFormat
 *
 *  Description:
 *      Validates a format structure.
 *
 *  Arguments:
 *      LPCWAVEFORMATEX [in]: format.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "WaveFormat::IsValidFormat"

BOOL
WaveFormat::IsValidFormat
(
    LPCWAVEFORMATEX         pwfx
)
{
    BOOL                    fSuccess;
    
    DPF_ENTER();

    ASSERT(pwfx);

    switch(pwfx->wFormatTag)
    {
        case WAVE_FORMAT_PCM:
            fSuccess = IsValidPcmFormat(pwfx);
            break;

        case WAVE_FORMAT_XBOX_ADPCM:
            fSuccess = IsValidAdpcmFormat((LPCXBOXADPCMWAVEFORMAT)pwfx);
            break;

#if defined(_XBOX) && !defined(MCPX_BOOT_LIB)

        case WAVE_FORMAT_EXTENSIBLE:
            fSuccess = IsValidExtensibleFormat((LPCWAVEFORMATEXTENSIBLE)pwfx);
            break;

#endif // defined(_XBOX) && !defined(MCPX_BOOT_LIB)

        default:
            DPF_ERROR("Unsupported format tag");
            fSuccess = FALSE;
            break;
    }

    DPF_LEAVE(fSuccess);

    return fSuccess;
}
            

/****************************************************************************
 *
 *  PcmToInternalFormat
 *
 *  Description:
 *      Initializes a DSWAVEFORMAT structure based on a PCMWAVEFORMAT 
 *      structure.
 *
 *  Arguments:
 *      LPDSWAVEFORMAT [out]: DirectSound format structure.
 *      LPCWAVEFORMATEX [in]: generic format structure.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "WaveFormat::PcmToInternalFormat"

void
WaveFormat::PcmToInternalFormat
(
    LPDSWAVEFORMAT          pDest,
    LPCWAVEFORMATEX         pSource
)
{
    DPF_ENTER();

    ASSERT(pDest);
    ASSERT(pSource);

    ASSERT(pSource->nChannels <= 0xFF);
    ASSERT(pSource->wBitsPerSample <= 0xFF);
    ASSERT(pSource->nBlockAlign <= 0xFF);

    pDest->wFormatTag = WAVE_FORMAT_PCM;
    pDest->nChannels = (BYTE)pSource->nChannels;
    pDest->wBitsPerSample = (BYTE)pSource->wBitsPerSample;
    pDest->nSamplesPerSec = pSource->nSamplesPerSec;
    pDest->nBlockAlign = (BYTE)pSource->nBlockAlign;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  IsValidPcmFormat
 *
 *  Description:
 *      Validates a format structure.
 *
 *  Arguments:
 *      LPCWAVEFORMATEX [in]: format.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "WaveFormat::IsValidPcmFormat"

BOOL 
WaveFormat::IsValidPcmFormat
(
    LPCWAVEFORMATEX         pwfx
)
{
    BOOL                    fValid  = TRUE;

    DPF_ENTER();

    ASSERT(pwfx);

    if(WAVE_FORMAT_PCM != pwfx->wFormatTag)
    {
        DPF_ERROR("Format tag not PCM");
        fValid = FALSE;
    }
    
    if((1 != pwfx->nChannels) && (2 != pwfx->nChannels) && (4 != pwfx->nChannels) && (6 != pwfx->nChannels))
    {
        DPF_ERROR("Invalid channel count");
        fValid = FALSE;
    }

    if((8 != pwfx->wBitsPerSample) && (16 != pwfx->wBitsPerSample))
    {
        DPF_ERROR("Invalid sample bit resolution");
        fValid = FALSE;
    }

    if((pwfx->nSamplesPerSec < DSBFREQUENCY_MIN) || (pwfx->nSamplesPerSec > DSBFREQUENCY_MAX))
    {
        DPF_ERROR("Invalid sampling rate");
        fValid = FALSE;
    }

    if(pwfx->nChannels * pwfx->wBitsPerSample / 8 != pwfx->nBlockAlign)
    {
        DPF_ERROR("Bad block alignment");
        fValid = FALSE;
    }

    DPF_LEAVE(fValid);

    return fValid;
}


/****************************************************************************
 *
 *  AdpcmToInternalFormat
 *
 *  Description:
 *      Initializes a DSWAVEFORMAT structure based on an XBOXADPCMWAVEFORMAT 
 *      structure.
 *
 *  Arguments:
 *      LPDSWAVEFORMAT [out]: DirectSound format structure.
 *      LPCXBOXADPCMWAVEFORMAT [in]: generic format structure.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "WaveFormat::AdpcmToInternalFormat"

void
WaveFormat::AdpcmToInternalFormat
(
    LPDSWAVEFORMAT          pDest,
    LPCXBOXADPCMWAVEFORMAT  pSource
)
{
    static const BYTE       wBitsPerSample      = 4;

    DPF_ENTER();

    ASSERT(pDest);
    ASSERT(pSource);

    ASSERT(pSource->wfx.nChannels <= 0xFF);
    ASSERT(pSource->wfx.wBitsPerSample <= 0xFF);
    ASSERT(pSource->wfx.nBlockAlign <= 0xFF);

    pDest->wFormatTag = WAVE_FORMAT_XBOX_ADPCM;
    pDest->nChannels = (BYTE)pSource->wfx.nChannels;
    pDest->wBitsPerSample = (BYTE)wBitsPerSample;
    pDest->nSamplesPerSec = pSource->wfx.nSamplesPerSec;
    pDest->nBlockAlign = (BYTE)pSource->wfx.nBlockAlign;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  IsValidAdpcmFormat
 *
 *  Description:
 *      Validates a format structure.
 *
 *  Arguments:
 *      LPCXBOXADPCMWAVEFORMAT [in]: format.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "WaveFormat::IsValidAdpcmFormat"

BOOL 
WaveFormat::IsValidAdpcmFormat
(
    LPCXBOXADPCMWAVEFORMAT  pwfx
)
{
    static const WORD       wSamplesPerBlock    = 64;
    static const WORD       wBitsPerSample      = 4;
    static const WORD       wBytesPerBlock      = 36;
    BOOL                    fValid              = TRUE;

    DPF_ENTER();

    ASSERT(pwfx);

    if(WAVE_FORMAT_XBOX_ADPCM != pwfx->wfx.wFormatTag)
    {
        DPF_ERROR("Format tag not XBOX_ADPCM");
        fValid = FALSE;
    }
    
    if(sizeof(*pwfx) - sizeof(pwfx->wfx) != pwfx->wfx.cbSize)
    {
        DPF_ERROR("cbSize not %lu", sizeof(*pwfx) - sizeof(pwfx->wfx));
        fValid = FALSE;
    }
    
    if((pwfx->wfx.nChannels < 1) || (pwfx->wfx.nChannels > 2))
    {
        DPF_ERROR("Invalid channel count");
        fValid = FALSE;
    }

    if((pwfx->wfx.nSamplesPerSec < DSBFREQUENCY_MIN) || (pwfx->wfx.nSamplesPerSec > DSBFREQUENCY_MAX))
    {
        DPF_ERROR("Invalid sampling rate");
        fValid = FALSE;
    }

    if(wBitsPerSample != pwfx->wfx.wBitsPerSample)
    {
        DPF_ERROR("Invalid sample bit resolution");
        fValid = FALSE;
    }

    if(wSamplesPerBlock != pwfx->wSamplesPerBlock)
    {
        DPF_ERROR("Invalid samples per block");
        fValid = FALSE;
    }

    if(wBytesPerBlock * pwfx->wfx.nChannels != pwfx->wfx.nBlockAlign)
    {
        DPF_ERROR("Bad block-alignment");
        fValid = FALSE;
    }

    DPF_LEAVE(fValid);

    return fValid;
}


/****************************************************************************
 *
 *  ExtensibleToInternalFormat
 *
 *  Description:
 *      Initializes a DSWAVEFORMAT structure based on a WAVEFORMATEXTENSIBLE
 *      structure.
 *
 *  Arguments:
 *      LPDSWAVEFORMAT [out]: DirectSound format structure.
 *      LPCWAVEFORMATEXTENSIBLE [in]: generic format structure.
 *
 *  Returns:  
 *      DWORD: mixbin assignment mask, if any.
 *
 ****************************************************************************/

#if defined(_XBOX) && !defined(MCPX_BOOT_LIB)

#undef DPF_FNAME
#define DPF_FNAME "WaveFormat::ExtensibleToInternalFormat"

DWORD
WaveFormat::ExtensibleToInternalFormat
(
    LPDSWAVEFORMAT          pDest,
    LPCWAVEFORMATEXTENSIBLE pSource
)
{
    DPF_ENTER();

    ASSERT(pDest);
    ASSERT(pSource);

    if(IsEqualGUID(pSource->SubFormat, KSDATAFORMAT_SUBTYPE_PCM))
    {
        PcmToInternalFormat(pDest, &pSource->Format);
    }
    else if(IsEqualGUID(pSource->SubFormat, KSDATAFORMAT_SUBTYPE_XBOX_ADPCM))
    {
        AdpcmToInternalFormat(pDest, (LPCXBOXADPCMWAVEFORMAT)&pSource->Format);
    }
    else
    {
        ASSERTMSG("Unexpected subformat");
    }

    DPF_LEAVE(pSource->dwChannelMask);

    return pSource->dwChannelMask;
}

#endif // defined(_XBOX) && !defined(MCPX_BOOT_LIB)


/****************************************************************************
 *
 *  IsValidExtensibleFormat
 *
 *  Description:
 *      Validates a format structure.
 *
 *  Arguments:
 *      LPCWAVEFORMATEXTENSIBLE [in]: format.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#if defined(_XBOX) && !defined(MCPX_BOOT_LIB)

#undef DPF_FNAME
#define DPF_FNAME "WaveFormat::IsValidExtensibleFormat"

BOOL 
WaveFormat::IsValidExtensibleFormat
(
    LPCWAVEFORMATEXTENSIBLE pwfx
)
{
    BOOL                    fValid  = TRUE;

    DPF_ENTER();

    ASSERT(pwfx);

    if(WAVE_FORMAT_EXTENSIBLE != pwfx->Format.wFormatTag)
    {
        DPF_ERROR("Format tag not EXTENSIBLE");
        fValid = FALSE;
    }
    
    if(sizeof(*pwfx) - sizeof(pwfx->Format) != pwfx->Format.cbSize)
    {
        DPF_ERROR("cbSize not %lu", sizeof(*pwfx) - sizeof(pwfx->Format));
        fValid = FALSE;
    }
    
    if(IsEqualGUID(pwfx->SubFormat, KSDATAFORMAT_SUBTYPE_PCM))
    {
        if(!IsValidPcmFormat(&pwfx->Format))
        {
            fValid = FALSE;
        }

        if(pwfx->Samples.wValidBitsPerSample != pwfx->Format.wBitsPerSample)
        {
            DPF_ERROR("wValidBitsPerSample must equal wBitsPerSample");
            fValid = FALSE;
        }

        if(pwfx->Samples.wReserved)
        {
            DPF_ERROR("wReserved must be 0 for SUBTYPE_PCM");
            fValid = FALSE;
        }
    }
    else if(IsEqualGUID(pwfx->SubFormat, KSDATAFORMAT_SUBTYPE_XBOX_ADPCM))
    {
        if(!IsValidAdpcmFormat((LPCXBOXADPCMWAVEFORMAT)&pwfx->Format))
        {
            fValid = FALSE;
        }
    }
    else
    {
        DPF_ERROR("Invalid sub-format identifier");
        fValid = FALSE;
    }

    if(pwfx->dwChannelMask & ~SPEAKER_MASK)
    {
        DPF_ERROR("Invalid channel mask");
        fValid = FALSE;
    }

    DPF_LEAVE(fValid);

    return fValid;
}

#endif // defined(_XBOX) && !defined(MCPX_BOOT_LIB)


