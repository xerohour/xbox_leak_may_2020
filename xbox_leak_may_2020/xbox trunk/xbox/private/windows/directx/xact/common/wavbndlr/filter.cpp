/***************************************************************************
 *
 *  Copyright (C) 11/27/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       filter.cpp
 *  Content:    Filter classes.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/27/2001   dereks  Created.
 *
 ****************************************************************************/

#include "wavbndli.h"


/****************************************************************************
 *
 *  CXboxAdpcmFilter
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
#define DPF_FNAME "CXboxAdpcmFilter::CXboxAdpcmFilter"

CXboxAdpcmFilter::CXboxAdpcmFilter
(
    void
)
{
}


/****************************************************************************
 *
 *  ~CXboxAdpcmFilter
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
#define DPF_FNAME "CXboxAdpcmFilter::~CXboxAdpcmFilter"

CXboxAdpcmFilter::~CXboxAdpcmFilter
(
    void
)
{
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      LPCWAVEBANKMINIWAVEFORMAT [in]: source format.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CXboxAdpcmFilter::Initialize"

BOOL
CXboxAdpcmFilter::Initialize
(
    LPCWAVEBANKMINIWAVEFORMAT   pwfxSource
)
{
    WAVEBANKMINIWAVEFORMAT      wfxDest     = *pwfxSource;
    WAVEBANKUNIWAVEFORMAT       wfxExpanded;

    if(!ConvertFormat(&wfxDest))
    {
        return FALSE;
    }

    if(!WaveBankExpandFormat(&wfxDest, &wfxExpanded))
    {
        return FALSE;
    }

    return CImaAdpcmCodec::Initialize(&wfxExpanded.AdpcmWaveFormat, TRUE);
}


/****************************************************************************
 *
 *  IsValidFormat
 *
 *  Description:
 *      Checks a source format for validity.
 *
 *  Arguments:
 *      LPWAVEBANKMINIWAVEFORMAT [in/out]: format.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CXboxAdpcmFilter::IsValidFormat"

BOOL 
CXboxAdpcmFilter::IsValidFormat
(
    LPCWAVEBANKMINIWAVEFORMAT  pwfxFormat
)
{
    if(WAVEBANKMINIFORMAT_TAG_PCM != pwfxFormat->wFormatTag)
    {
        return FALSE;
    }

    if((pwfxFormat->nChannels < 1) || (pwfxFormat->nChannels > 2))
    {
        return FALSE;
    }

    if(WAVEBANKMINIFORMAT_BITDEPTH_16 != pwfxFormat->wBitsPerSample)
    {
        return FALSE;
    }

    return TRUE;
}


/****************************************************************************
 *
 *  ConvertFormat
 *
 *  Description:
 *      Converts a source format to a destination.
 *
 *  Arguments:
 *      LPWAVEBANKMINIWAVEFORMAT [in/out]: format.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CXboxAdpcmFilter::ConvertFormat"

BOOL 
CXboxAdpcmFilter::ConvertFormat
(
    LPWAVEBANKMINIWAVEFORMAT   pwfxFormat
)
{
    if(!IsValidFormat(pwfxFormat))
    {
        return FALSE;
    }

    pwfxFormat->wFormatTag = WAVEBANKMINIFORMAT_TAG_ADPCM;
    pwfxFormat->wBitsPerSample = WAVEBANKMINIFORMAT_BITDEPTH_4;

    return TRUE;
}


/****************************************************************************
 *
 *  Convert
 *
 *  Description:
 *      Converts data.
 *
 *  Arguments:
 *      LPCVOID [in]: source buffer.
 *      DWORD [in/out]: source buffer size, in bytes.
 *      LPVOID [out]: destination buffer.
 *      DWORD [in/out]: destination buffer size, in bytes.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CXboxAdpcmFilter::Convert"

BOOL
CXboxAdpcmFilter::Convert
(
    LPCVOID                 pvSource,
    LPDWORD                 pcbSource,
    LPVOID                  pvDest,
    LPDWORD                 pcbDest
)
{
    DWORD                   cBlocks;

    //
    // Convert byte sizes to blocks
    //

    *pcbSource /= m_wfxEncode.wfx.nChannels * 2 * XBOX_ADPCM_SAMPLES_PER_BLOCK;
    *pcbDest /= m_wfxEncode.wfx.nChannels * XBOX_ADPCM_ENCODED_BLOCK_SIZE;
    
    cBlocks = min(*pcbSource, *pcbDest);

    *pcbSource = cBlocks * m_wfxEncode.wfx.nChannels * 2 * XBOX_ADPCM_SAMPLES_PER_BLOCK;
    *pcbDest = cBlocks * m_wfxEncode.wfx.nChannels * XBOX_ADPCM_ENCODED_BLOCK_SIZE;

    //
    // Convert data
    //

    return CImaAdpcmCodec::Convert(pvSource, pvDest, cBlocks);
}


/****************************************************************************
 *
 *  C8BitFilter
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
#define DPF_FNAME "C8BitFilter::C8BitFilter"

C8BitFilter::C8BitFilter
(
    void
)
{
}


/****************************************************************************
 *
 *  ~C8BitFilter
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
#define DPF_FNAME "C8BitFilter::~C8BitFilter"

C8BitFilter::~C8BitFilter
(
    void
)
{
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      LPCWAVEBANKMINIWAVEFORMAT [in]: source format.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "C8BitFilter::Initialize"

BOOL
C8BitFilter::Initialize
(
    LPCWAVEBANKMINIWAVEFORMAT  pwfxSource
)
{
    if(!IsValidFormat(pwfxSource))
    {
        return FALSE;
    }
    
    m_nChannels = pwfxSource->nChannels;
    
    return TRUE;
}


/****************************************************************************
 *
 *  IsValidFormat
 *
 *  Description:
 *      Checks a source format for validity.
 *
 *  Arguments:
 *      LPWAVEBANKMINIWAVEFORMAT [in/out]: format.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "C8BitFilter::IsValidFormat"

BOOL 
C8BitFilter::IsValidFormat
(
    LPCWAVEBANKMINIWAVEFORMAT  pwfxFormat
)
{
    if(WAVEBANKMINIFORMAT_TAG_PCM != pwfxFormat->wFormatTag)
    {
        return FALSE;
    }

    if(WAVEBANKMINIFORMAT_BITDEPTH_16 != pwfxFormat->wBitsPerSample)
    {
        return FALSE;
    }

    return TRUE;
}


/****************************************************************************
 *
 *  ConvertFormat
 *
 *  Description:
 *      Converts a source format to a destination.
 *
 *  Arguments:
 *      LPWAVEBANKMINIWAVEFORMAT [in/out]: format.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "C8BitFilter::ConvertFormat"

BOOL 
C8BitFilter::ConvertFormat
(
    LPWAVEBANKMINIWAVEFORMAT   pwfxFormat
)
{
    if(!IsValidFormat(pwfxFormat))
    {
        return FALSE;
    }

    pwfxFormat->wBitsPerSample = WAVEBANKMINIFORMAT_BITDEPTH_8;

    return TRUE;
}


/****************************************************************************
 *
 *  Convert
 *
 *  Description:
 *      Converts data.
 *
 *  Arguments:
 *      LPCVOID [in]: source buffer.
 *      DWORD [in/out]: source buffer size, in bytes.
 *      LPVOID [out]: destination buffer.
 *      DWORD [in/out]: destination buffer size, in bytes.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "C8BitFilter::Convert"

BOOL
C8BitFilter::Convert
(
    LPCVOID                 pvSource,
    LPDWORD                 pcbSource,
    LPVOID                  pvDest,
    LPDWORD                 pcbDest
)
{
    const short *           psSource    = (const short *)pvSource;
    unsigned char *         pbDest      = (unsigned char *)pvDest;
    DWORD                   cSamples;
    DWORD                   dwSample;

    //
    // Convert byte sizes to samples
    //

    *pcbSource /= m_nChannels * 2;
    *pcbDest /= m_nChannels;
    
    cSamples = min(*pcbSource, *pcbDest);

    *pcbSource = cSamples * m_nChannels * 2;
    *pcbDest = cSamples * m_nChannels;

    //
    // Convert data
    //

    cSamples *= m_nChannels;
    
    while(cSamples--)
    {
        dwSample = *psSource + 32768;
        *pbDest = (BYTE)(dwSample >> 8);

        pbDest++;
        psSource++;
    }

    return TRUE;
}


