/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       format.c
 *  Content:    WAVEFORMATEX helper functions.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  10/10/00    dereks  Created.
 *
 ****************************************************************************/

#include "dsoundi.h"


/****************************************************************************
 *
 *  XAudioCreatePcmFormat
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
#define DPF_FNAME "XAudioCreatePcmFormat"

void 
XAudioCreatePcmFormat
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
 *  XAudioCreateAdpcmFormat
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
#define DPF_FNAME "XAudioCreateAdpcmFormat"

void 
XAudioCreateAdpcmFormat
(
    WORD                    nChannels, 
    DWORD                   nSamplesPerSec, 
    LPXBOXADPCMWAVEFORMAT   pwfx
)
{
    static const WORD       wSamplesPerBlock    = 64;
    static const WORD       wBitsPerSample      = 4;
    static const WORD       wBytesPerBlock      = 36;
    
    DPF_ENTER();
    
    ASSERT(pwfx);

    pwfx->wfx.wFormatTag = WAVE_FORMAT_XBOX_ADPCM;
    pwfx->wfx.nChannels = nChannels;
    pwfx->wfx.nSamplesPerSec = nSamplesPerSec;
    pwfx->wfx.wBitsPerSample = wBitsPerSample;
    pwfx->wfx.nBlockAlign = nChannels * wBytesPerBlock;
    pwfx->wfx.nAvgBytesPerSec = nSamplesPerSec / wSamplesPerBlock * wBytesPerBlock;
    pwfx->wfx.cbSize = sizeof(*pwfx) - sizeof(pwfx->wfx);
    pwfx->wSamplesPerBlock = wSamplesPerBlock;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  FmtCreateInternal
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
#define DPF_FNAME "FmtCreateInternal"

DWORD
FmtCreateInternal
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
            FmtPcmToInternal(pDest, pSource);
            break;

        case WAVE_FORMAT_XBOX_ADPCM:
            FmtAdpcmToInternal(pDest, (LPCXBOXADPCMWAVEFORMAT)pSource);
            break;

#ifndef MCPX_BOOT_LIB

        case WAVE_FORMAT_EXTENSIBLE:
            dwChannelMask = FmtExtensibleToInternal(pDest, (LPCWAVEFORMATEXTENSIBLE)pSource);
            break;

#endif // MCPX_BOOT_LIB

        default:
            ASSERTMSG("Unexpected format tag");
            break;
    }

    DPF_LEAVE(dwChannelMask);

    return dwChannelMask;
}
            

/****************************************************************************
 *
 *  FmtIsValid
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
#define DPF_FNAME "FmtIsValid"

BOOL
FmtIsValid
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
            fSuccess = FmtIsValidPcm(pwfx);
            break;

        case WAVE_FORMAT_XBOX_ADPCM:
            fSuccess = FmtIsValidAdpcm((LPCXBOXADPCMWAVEFORMAT)pwfx);
            break;

#ifndef MCPX_BOOT_LIB

        case WAVE_FORMAT_EXTENSIBLE:
            fSuccess = FmtIsValidExtensible((LPCWAVEFORMATEXTENSIBLE)pwfx);
            break;

#endif // MCPX_BOOT_LIB

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
 *  FmtPcmToInternal
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
#define DPF_FNAME "FmtPcmToInternal"

void
FmtPcmToInternal
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
 *  FmtIsValidPcm
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
#define DPF_FNAME "FmtIsValidPcm"

BOOL 
FmtIsValidPcm
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
 *  FmtAdpcmToInternal
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
#define DPF_FNAME "FmtAdpcmToInternal"

void
FmtAdpcmToInternal
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
 *  FmtIsValidAdpcm
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
#define DPF_FNAME "FmtIsValidAdpcm"

BOOL 
FmtIsValidAdpcm
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
 *  FmtExtensibleToInternal
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

#ifndef MCPX_BOOT_LIB

#undef DPF_FNAME
#define DPF_FNAME "FmtExtensibleToInternal"

DWORD
FmtExtensibleToInternal
(
    LPDSWAVEFORMAT          pDest,
    LPCWAVEFORMATEXTENSIBLE pSource
)
{
    DPF_ENTER();

    ASSERT(pDest);
    ASSERT(pSource);

    if(IsEqualGUID(&pSource->SubFormat, &KSDATAFORMAT_SUBTYPE_PCM))
    {
        FmtPcmToInternal(pDest, &pSource->Format);
    }
    else if(IsEqualGUID(&pSource->SubFormat, &KSDATAFORMAT_SUBTYPE_XBOX_ADPCM))
    {
        FmtAdpcmToInternal(pDest, (LPCXBOXADPCMWAVEFORMAT)&pSource->Format);
    }
    else
    {
        ASSERTMSG("Unexpected subformat");
    }

    DPF_LEAVE(pSource->dwChannelMask);

    return pSource->dwChannelMask;
}

#endif // MCPX_BOOT_LIB


/****************************************************************************
 *
 *  FmtIsValidExtensible
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

#ifndef MCPX_BOOT_LIB

#undef DPF_FNAME
#define DPF_FNAME "FmtIsValidExtensible"

BOOL 
FmtIsValidExtensible
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
    
    if(IsEqualGUID(&pwfx->SubFormat, &KSDATAFORMAT_SUBTYPE_PCM))
    {
        if(!FmtIsValidPcm(&pwfx->Format))
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
    else if(IsEqualGUID(&pwfx->SubFormat, &KSDATAFORMAT_SUBTYPE_XBOX_ADPCM))
    {
        if(!FmtIsValidAdpcm((LPCXBOXADPCMWAVEFORMAT)&pwfx->Format))
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

#endif // MCPX_BOOT_LIB


