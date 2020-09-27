/***************************************************************************
 *
 *  Copyright (C) 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       driver.cpp
 *  Content:    XBox ADPCM ACM driver.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  04/29/01    dereks  Created.
 *
 ****************************************************************************/

#include "imaadpcm.h"
#include "resource.h"
#include "stdio.h"

#if DBG
#define DPF DbgPrint
#else
#pragma warning(disable:4002)
#define DPF()
#endif

#define XBOX_ADPCM_SAMPLES_PER_BLOCK    64

#define VERSION_ACM_DRIVER              MAKE_ACM_VERSION(4,  0, 0)
#define VERSION_MSACM                   MAKE_ACM_VERSION(3, 50, 0)

enum
{
    XBOX_ADPCM_FORMAT_INDEX_PCM = 0,
    XBOX_ADPCM_FORMAT_INDEX_ADPCM,
    XBOX_ADPCM_FORMAT_INDEX_COUNT,
};

//
// The driver object
//

class CXboxAdpcmDriver
{
public:
    static const DWORD      m_adwStdSamplingRates[];    // "Standard" sampling rates used by this CODEC
    static DWORD            m_dwOpenDriverCount;        // Count of driver instances currently open

public:
    // Driver functions
    static LONG OpenDriver(HDRVR hdrvr, LPACMDRVOPENDESC pOpen);
    static LONG CloseDriver(HDRVR hdrvr, DWORD dwDriverId);
    static LONG GetDriverDetails(HDRVR hdrvr, LPACMDRIVERDETAILS pDetails);
    static LONG GetFormatTagDetails(HDRVR hdrvr, LPACMFORMATTAGDETAILS pDetails, DWORD dwFlags);
    static LONG GetFormatDetails(HDRVR hdrvr, LPACMFORMATDETAILS pDetails, DWORD dwFlags);
    static LONG SuggestFormat(HDRVR hdrvr, LPACMDRVFORMATSUGGEST pSuggest);
    static LONG OpenStream(HDRVR hdrvr, LPACMDRVSTREAMINSTANCE pStream);
    static LONG CloseStream(HDRVR hdrvr, LPACMDRVSTREAMINSTANCE pStream);
    static LONG GetStreamSize(HDRVR hdrvr, LPACMDRVSTREAMINSTANCE pStream, LPACMDRVSTREAMSIZE pSize);
    static LONG ConvertStream(HDRVR hdrvr, LPACMDRVSTREAMINSTANCE pStream, LPACMDRVSTREAMHEADER pHeader);

    // Format helpers
    static BOOL IsValidPcmFormat(LPCWAVEFORMATEX pwfx);
    static BOOL IsValidAdpcmFormat(LPCIMAADPCMWAVEFORMAT pwfx);

    static void CreatePcmFormat(WORD nChannels, DWORD nSamplesPerSec, LPWAVEFORMATEX pwfx);
    static void CreateAdpcmFormat(WORD nChannels, DWORD nSamplesPerSec, LPIMAADPCMWAVEFORMAT pwfx);
};

const DWORD CXboxAdpcmDriver::m_adwStdSamplingRates[] =
{
    8000,
    11025,
    16000,
    22050,
    32000,
    44100,
    48000
};

DWORD CXboxAdpcmDriver::m_dwOpenDriverCount = 0;

__inline BOOL CXboxAdpcmDriver::IsValidPcmFormat(LPCWAVEFORMATEX pwfx)
{
    return CImaAdpcmCodec::IsValidPcmFormat(pwfx);
}

__inline BOOL CXboxAdpcmDriver::IsValidAdpcmFormat(LPCIMAADPCMWAVEFORMAT pwfx)
{
    if(!CImaAdpcmCodec::IsValidImaAdpcmFormat(pwfx))
    {
        return FALSE;
    }

    if(XBOX_ADPCM_SAMPLES_PER_BLOCK != pwfx->wSamplesPerBlock)
    {
        return FALSE;
    }

    return TRUE;
}

__inline void CXboxAdpcmDriver::CreatePcmFormat(WORD nChannels, DWORD nSamplesPerSec, LPWAVEFORMATEX pwfx)
{
    CImaAdpcmCodec::CreatePcmFormat(nChannels, nSamplesPerSec, pwfx);
}

__inline void CXboxAdpcmDriver::CreateAdpcmFormat(WORD nChannels, DWORD nSamplesPerSec, LPIMAADPCMWAVEFORMAT pwfx)
{
    CImaAdpcmCodec::CreateImaAdpcmFormat(nChannels, nSamplesPerSec, XBOX_ADPCM_SAMPLES_PER_BLOCK, pwfx);
}


/****************************************************************************
 *
 *  DbgPrint
 *
 *  Description:
 *      Prints a string to the debugger.
 *
 *  Arguments:
 *      LPCSTR [in]: format string.
 *      ... [in]: format arguments.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

void __cdecl 
DbgPrint
(
    LPCSTR                  pszFormat,
    ...
)
{
    CHAR                    szFinal[0x400];
    va_list                 va;

    OutputDebugString("XBADPCM: ");
    
    va_start(va, pszFormat);
    vsprintf(szFinal, pszFormat, va);
    va_end(va);

    strcat(szFinal, "\n");

    OutputDebugStringA(szFinal);

}


/****************************************************************************
 *
 *  MyLoadStringW
 *
 *  Description:
 *      Loads a unicode version of a resource string.
 *
 *  Arguments:
 *      HINSTANCE [in]: module instance handle.
 *      UINT [in]: resource identifier.
 *      LPWSTR [in]: resource buffer.
 *      int [in]: size of buffer.
 *
 *  Returns:  
 *      int: number of characters copied to the buffer.
 *
 ****************************************************************************/
 
#ifndef UNICODE

int
MyLoadStringW
(
    HINSTANCE               hInst,
    UINT                    nResourceId,
    LPWSTR                  pszString,
    int                     nStringLength
)
{
    CHAR                    szAnsi[0x100];

    //
    // Load the ANSI version
    //

    if(!LoadStringA(hInst, nResourceId, szAnsi, NUMELMS(szAnsi)))
    {
        return 0;
    }

    //
    // Convert to UNICODE
    //

    return MultiByteToWideChar(CP_ACP, 0, szAnsi, -1, pszString, nStringLength);
}

#define LoadStringW MyLoadStringW

#endif // UNICODE


/****************************************************************************
 *
 *  DriverProc
 *
 *  Description:
 *      Driver entry point.
 *
 *  Arguments:
 *      DWORD [in]: driver identifier.
 *      HDRVR [in]: driver handle.
 *      UINT [in]: driver message.
 *      LPARAM [in]: message parameter 1.
 *      LPARAM [in]: message parameter 2.
 *
 *  Returns:  
 *      LONG: non-zero on success.
 *
 ****************************************************************************/

EXTERN_C
LONG
CALLBACK
DriverProc
(
    DWORD                   dwDriverId, 
    HDRVR                   hdrvr,
    UINT                    uMsg,
    LPARAM                  lParam1,
    LPARAM                  lParam2
)
{
    if(uMsg >= ACMDM_BASE)
    {
        DPF("ACM message %lu:  %x %x %x %x", uMsg - ACMDM_BASE, dwDriverId, hdrvr, lParam1, lParam2);
    }
    else
    {
        DPF("System message %lu:  %x %x %x %x", uMsg, dwDriverId, hdrvr, lParam1, lParam2);
    }

    //
    // Handle generic driver messages
    //

    switch(uMsg)
    {
        case DRV_CONFIGURE:
        case DRV_QUERYCONFIGURE:
            
            //
            // We're not supporting configuration
            //

            return 0;

        case DRV_LOAD:
        case DRV_FREE:
            
            //
            // Ok, thanks.  We're ready to go
            //

            return 1;

        case DRV_DISABLE:
        case DRV_ENABLE:
        case DRV_POWER:
            
            //
            // Ignored
            //

            return 0;

        case DRV_INSTALL:
        case DRV_REMOVE:

            //
            // No extra install necessary
            //

            return DRVCNF_OK;

        case ACMDM_DRIVER_ABOUT:

            //
            // Not supporting an about box
            //

            return MMSYSERR_NOTSUPPORTED;
    }

    //
    // Handle the specific message
    //

    switch(uMsg)
    {
        case DRV_OPEN:
            
            //
            // Handle the open operation
            //

            return CXboxAdpcmDriver::OpenDriver(hdrvr, (LPACMDRVOPENDESC)lParam2);

        case DRV_CLOSE:
            
            //
            // Close the driver instance
            //
            
            return CXboxAdpcmDriver::CloseDriver(hdrvr, dwDriverId);

        case ACMDM_DRIVER_DETAILS:
            
            //
            // Get driver details
            //

            return CXboxAdpcmDriver::GetDriverDetails(hdrvr, (LPACMDRIVERDETAILS)lParam1);

        case ACMDM_FORMAT_SUGGEST:

            //
            // Suggest a format
            //

            return CXboxAdpcmDriver::SuggestFormat(hdrvr, (LPACMDRVFORMATSUGGEST)lParam1);

        case ACMDM_FORMATTAG_DETAILS:

            //
            // Get format tag details
            //

            return CXboxAdpcmDriver::GetFormatTagDetails(hdrvr, (LPACMFORMATTAGDETAILS)lParam1, (DWORD)lParam2);

        case ACMDM_FORMAT_DETAILS:
            
            //
            // Get format details
            //

            return CXboxAdpcmDriver::GetFormatDetails(hdrvr, (LPACMFORMATDETAILS)lParam1, (DWORD)lParam2);

        case ACMDM_STREAM_OPEN:
            
            //
            // Open a new stream
            //

            return CXboxAdpcmDriver::OpenStream(hdrvr, (LPACMDRVSTREAMINSTANCE)lParam1);

        case ACMDM_STREAM_CLOSE:
            
            //
            // Close an open stream
            //

            return CXboxAdpcmDriver::CloseStream(hdrvr, (LPACMDRVSTREAMINSTANCE)lParam1);

        case ACMDM_STREAM_SIZE:
            
            //
            // Get the size of converted data
            //

            return CXboxAdpcmDriver::GetStreamSize(hdrvr, (LPACMDRVSTREAMINSTANCE)lParam1, (LPACMDRVSTREAMSIZE)lParam2);

        case ACMDM_STREAM_CONVERT:

            //
            // Convert a stream of data
            //

            return CXboxAdpcmDriver::ConvertStream(hdrvr, (LPACMDRVSTREAMINSTANCE)lParam1, (LPACMDRVSTREAMHEADER)lParam2);
    }

    return DefDriverProc(dwDriverId, hdrvr, uMsg, lParam1, lParam2);
}


/****************************************************************************
 *
 *  OpenDriver
 *
 *  Description:
 *      Initializes the driver object.
 *
 *  Arguments:
 *      LPACMDRVOPENDESC [in/out]: ACM data.
 *
 *  Returns:  
 *      LONG: driver instance.
 *
 ****************************************************************************/

LONG
CXboxAdpcmDriver::OpenDriver
(
    HDRVR                   hdrvr,
    LPACMDRVOPENDESC        pOpen
)
{
    HINSTANCE               hInst;

    //
    // Refuse to open if we're being called from anyone but ACM
    //

    if(pOpen)
    {
        if(ACMDRIVERDETAILS_FCCTYPE_AUDIOCODEC != pOpen->fccType)
        {
            return 0;
        }
    }

    //
    // Tell ACM that we loaded successfully
    //

    if(pOpen)
    {
        pOpen->dwError = MMSYSERR_NOERROR;
    }

    //
    // Increment the count of open driver instances
    //
    
    m_dwOpenDriverCount++;

    DPF("Driver %lu open", m_dwOpenDriverCount);

    return m_dwOpenDriverCount;
}


/****************************************************************************
 *
 *  CloseDriver
 *
 *  Description:
 *      Closes a driver instance.
 *
 *  Arguments:
 *      DWORD [in]: driver instance.
 *
 *  Returns:  
 *      LONG: non-zero on success.
 *
 ****************************************************************************/

LONG
CXboxAdpcmDriver::CloseDriver
(
    HDRVR                   hdrvr,
    DWORD                   dwDriverId
)
{
    DPF("CloseDriver %lu", dwDriverId);

    if(dwDriverId > m_dwOpenDriverCount)
    {
        DPF("Invalid driver identifier");
        return 0;
    }

    m_dwOpenDriverCount--;

    return 1;
}


/****************************************************************************
 *
 *  GetDriverDetails
 *
 *  Description:
 *      Gets information about the driver.
 *
 *  Arguments:
 *      HDRVR [in]: driver handle.
 *      LPACMDRIVERDETAILS [in/out]: driver details.
 *
 *  Returns:  
 *      LONG: MMSYSTEM error code.
 *
 ****************************************************************************/

LONG
CXboxAdpcmDriver::GetDriverDetails
(
    HDRVR                   hdrvr,
    LPACMDRIVERDETAILS      pDetails
)
{
    HINSTANCE               hInst;
    ACMDRIVERDETAILS        add;

    DPF("GetDriverDetails");

    hInst = GetDriverModuleHandle(hdrvr);

    ZeroMemory(&add, sizeof(add));
    
    add.cbStruct = sizeof(add);
    add.fccType = ACMDRIVERDETAILS_FCCTYPE_AUDIOCODEC;
    add.fccComp = ACMDRIVERDETAILS_FCCCOMP_UNDEFINED;
    add.wMid = MM_MICROSOFT;
    add.wPid = WAVE_FORMAT_XBOX_ADPCM; // BUGBUG
    add.vdwACM = VERSION_MSACM;
    add.vdwDriver = VERSION_ACM_DRIVER;
    add.fdwSupport = ACMDRIVERDETAILS_SUPPORTF_CODEC;
    add.cFormatTags = XBOX_ADPCM_FORMAT_INDEX_COUNT;

    LoadStringW(hInst, IDS_ACM_DRIVER_SHORTNAME, add.szShortName, NUMELMS(add.szShortName));
    LoadStringW(hInst, IDS_ACM_DRIVER_LONGNAME,  add.szLongName,  NUMELMS(add.szLongName));
    LoadStringW(hInst, IDS_ACM_DRIVER_COPYRIGHT, add.szCopyright, NUMELMS(add.szCopyright));
    LoadStringW(hInst, IDS_ACM_DRIVER_LICENSING, add.szLicensing, NUMELMS(add.szLicensing));
    LoadStringW(hInst, IDS_ACM_DRIVER_FEATURES,  add.szFeatures,  NUMELMS(add.szFeatures));

    CopyMemory(pDetails, &add, min(pDetails->cbStruct, add.cbStruct));

    return MMSYSERR_NOERROR;
}


/****************************************************************************
 *
 *  GetFormatTagDetails
 *
 *  Description:
 *      Gets information about formats supported by this driver.
 *
 *  Arguments:
 *      LPACMFORMATTAGDETAILS [in/out]: format tag details.
 *      DWORD [in]: flags.
 *
 *  Returns:  
 *      LONG: MMSYSTEM error code.
 *
 ****************************************************************************/

LONG
CXboxAdpcmDriver::GetFormatTagDetails
(
    HDRVR                   hdrvr,
    LPACMFORMATTAGDETAILS   pDetails,
    DWORD                   dwFlags
)
{
    HINSTANCE               hInst;
    DWORD                   dwFormatTag;
    
    DPF("GetFormatTagDetails");

    if(pDetails->cbStruct < sizeof(*pDetails))
    {
        DPF("Details structure too small");
        return ACMERR_NOTPOSSIBLE;
    }

    hInst = GetDriverModuleHandle(hdrvr);

    switch(dwFlags)
    {
        case ACM_FORMATTAGDETAILSF_INDEX:
            
            switch(pDetails->dwFormatTagIndex)
            {
                case XBOX_ADPCM_FORMAT_INDEX_PCM:
                    dwFormatTag = WAVE_FORMAT_PCM;
                    break;

                case XBOX_ADPCM_FORMAT_INDEX_ADPCM:
                    dwFormatTag = WAVE_FORMAT_XBOX_ADPCM;
                    break;

                default:
                    DPF("Bad format tag index");
                    return ACMERR_NOTPOSSIBLE;
            }
            
            break;

        case ACM_FORMATTAGDETAILSF_LARGESTSIZE:

            if(WAVE_FORMAT_UNKNOWN == pDetails->dwFormatTag)
            {
                dwFormatTag = WAVE_FORMAT_XBOX_ADPCM;
            }
            else
            {
                dwFormatTag = pDetails->dwFormatTag;
            }

            break;

        case ACM_FORMATTAGDETAILSF_FORMATTAG:
            dwFormatTag = pDetails->dwFormatTag;
            break;

        default:
            DPF("Bad tag details request");
            return MMSYSERR_NOTSUPPORTED;
    }

    switch(dwFormatTag)
    {
        case WAVE_FORMAT_PCM:
            DPF("Returning PCM tag details");
            
            pDetails->dwFormatTagIndex = XBOX_ADPCM_FORMAT_INDEX_PCM;
            pDetails->dwFormatTag = WAVE_FORMAT_PCM;
            pDetails->cbFormatSize = sizeof(PCMWAVEFORMAT);
            
            pDetails->szFormatTag[0] = 0;

            break;

        case WAVE_FORMAT_XBOX_ADPCM:
            DPF("Returning ADPCM tag details");
            
            pDetails->dwFormatTagIndex = XBOX_ADPCM_FORMAT_INDEX_ADPCM;
            pDetails->dwFormatTag = WAVE_FORMAT_XBOX_ADPCM;
            pDetails->cbFormatSize = sizeof(IMAADPCMWAVEFORMAT);

            LoadStringW(hInst, IDS_ACM_DRIVER_TAG_NAME, pDetails->szFormatTag, NUMELMS(pDetails->szFormatTag));

            break;

        default:
            DPF("Bad format tag");
            return ACMERR_NOTPOSSIBLE;
    }

    pDetails->cbStruct = sizeof(*pDetails);
    pDetails->fdwSupport = ACMDRIVERDETAILS_SUPPORTF_CODEC;
    pDetails->cStandardFormats = IMAADPCM_MAX_CHANNELS * NUMELMS(m_adwStdSamplingRates);

    return MMSYSERR_NOERROR;
}


/****************************************************************************
 *
 *  GetFormatDetails
 *
 *  Description:
 *      Gets information about formats supported by this driver.
 *
 *  Arguments:
 *      LPACMFORMATDETAILS [in/out]: format tag details.
 *      DWORD [in]: flags.
 *
 *  Returns:  
 *      LONG: MMSYSTEM error code.
 *
 ****************************************************************************/

LONG
CXboxAdpcmDriver::GetFormatDetails
(
    HDRVR                   hdrvr,
    LPACMFORMATDETAILS      pDetails,
    DWORD                   dwFlags
)
{
    HINSTANCE               hInst;
    DWORD                   dwSamplingRateIndex;
    DWORD                   dwSamplingRate;
    DWORD                   dwChannelCount;
    
    DPF("GetFormatDetails");

    if(pDetails->cbStruct < sizeof(*pDetails))
    {
        DPF("Details structure too small");
        return ACMERR_NOTPOSSIBLE;
    }

    hInst = GetDriverModuleHandle(hdrvr);

    switch(dwFlags)
    {
        case ACM_FORMATDETAILSF_INDEX:
            
            //
            // Fill in format by index
            //

            dwSamplingRateIndex = pDetails->dwFormatIndex / IMAADPCM_MAX_CHANNELS;

            if(dwSamplingRateIndex >= NUMELMS(m_adwStdSamplingRates))
            {
                return ACMERR_NOTPOSSIBLE;
            }

            dwChannelCount = (pDetails->dwFormatIndex % IMAADPCM_MAX_CHANNELS) + 1;
            dwSamplingRate = m_adwStdSamplingRates[dwSamplingRateIndex];

            switch(pDetails->dwFormatTag)
            {
                case WAVE_FORMAT_PCM:
                    
                    if(pDetails->cbwfx < sizeof(PCMWAVEFORMAT))
                    {
                        DPF("Format buffer too small for PCM");
                        return ACMERR_NOTPOSSIBLE;
                    }

                    DPF("Returning PCM details");

                    CreatePcmFormat((WORD)dwChannelCount, dwSamplingRate, pDetails->pwfx);

                    break;

                case WAVE_FORMAT_XBOX_ADPCM:

                    if(pDetails->cbwfx < sizeof(IMAADPCMWAVEFORMAT))
                    {
                        DPF("Format buffer too small for ADPCM");
                        return ACMERR_NOTPOSSIBLE;
                    }

                    DPF("Returning ADPCM details");

                    CreateAdpcmFormat((WORD)dwChannelCount, dwSamplingRate, (LPIMAADPCMWAVEFORMAT)pDetails->pwfx);

                    break;

                default:
                    DPF("Bad format tag");
                    return ACMERR_NOTPOSSIBLE;
            }

            break;

        case ACM_FORMATDETAILSF_FORMAT:

            //
            // Just validate the format
            //

            switch(pDetails->dwFormatTag)
            {
                case WAVE_FORMAT_PCM:

                    if(pDetails->cbwfx < sizeof(PCMWAVEFORMAT))
                    {
                        DPF("Format buffer too small for PCM");
                        return ACMERR_NOTPOSSIBLE;
                    }

                    if(!IsValidPcmFormat(pDetails->pwfx))
                    {
                        DPF("Bad PCM format");
                        return ACMERR_NOTPOSSIBLE;
                    }

                    break;

                case WAVE_FORMAT_XBOX_ADPCM:

                    if(pDetails->cbwfx < sizeof(IMAADPCMWAVEFORMAT))
                    {
                        DPF("Format buffer too small for ADPCM");
                        return ACMERR_NOTPOSSIBLE;
                    }

                    if(!IsValidAdpcmFormat((LPCIMAADPCMWAVEFORMAT)pDetails->pwfx))
                    {
                        DPF("Bad ADPCM format");
                        return ACMERR_NOTPOSSIBLE;
                    }

                    break;

                default:
                    DPF("Bad format tag");
                    return ACMERR_NOTPOSSIBLE;
            }

            break;

        default:
            DPF("Bad details request");
            return MMSYSERR_NOTSUPPORTED;
    }

    pDetails->cbStruct = sizeof(*pDetails);
    pDetails->fdwSupport = ACMDRIVERDETAILS_SUPPORTF_CODEC;
    pDetails->szFormat[0] = 0;

    return MMSYSERR_NOERROR;
}


/****************************************************************************
 *
 *  SuggestFormat
 *
 *  Description:
 *      Suggest a compatible format.
 *
 *  Arguments:
 *      LPACMDRVFORMATSUGGEST [in/out]: format suggestion data.
 *
 *  Returns:  
 *      LONG: MMSYSTEM error code.
 *
 ****************************************************************************/

LONG
CXboxAdpcmDriver::SuggestFormat
(
    HDRVR                   hdrvr,
    LPACMDRVFORMATSUGGEST   pSuggest
)
{
    static const DWORD      dwSupportMask   = ACM_FORMATSUGGESTF_WFORMATTAG | ACM_FORMATSUGGESTF_NCHANNELS | ACM_FORMATSUGGESTF_NSAMPLESPERSEC | ACM_FORMATSUGGESTF_WBITSPERSAMPLE;
    IMAADPCMWAVEFORMAT      wfxAdpcm;

    DPF("SuggestFormat");

    //
    // Make sure we support the requested fields
    //

    if(pSuggest->fdwSuggest & ~dwSupportMask)
    {
        return MMSYSERR_NOTSUPPORTED;
    }

    if(WAVE_FORMAT_PCM == pSuggest->pwfxSrc->wFormatTag)
    {
        //
        // Verify that the format is valid for our purposes
        //

        if(pSuggest->cbwfxSrc < sizeof(PCMWAVEFORMAT))
        {
            DPF("Format buffer too small for PCM");
            return ACMERR_NOTPOSSIBLE;
        }

        if(pSuggest->cbwfxDst < sizeof(WAVEFORMATEX))
        {
            DPF("Format buffer too small for ADPCM");
            return ACMERR_NOTPOSSIBLE;
        }

        if(!IsValidPcmFormat(pSuggest->pwfxSrc))
        {
            DPF("Bad PCM format");
            return ACMERR_NOTPOSSIBLE;
        }

        //
        // If the destination format tag is specified, make sure it's
        // ADPCM.
        //

        if(pSuggest->fdwSuggest & ACM_FORMATSUGGESTF_WFORMATTAG)
        {
            if(WAVE_FORMAT_XBOX_ADPCM != pSuggest->pwfxDst->wFormatTag)
            {
                DPF("Destination format tag not ADPCM");
                return ACMERR_NOTPOSSIBLE;
            }
        }

        //
        // If the destination channel count was specified, make sure
        // it's the same as the source.
        //

        if(pSuggest->fdwSuggest & ACM_FORMATSUGGESTF_NCHANNELS)
        {
            if(pSuggest->pwfxSrc->nChannels != pSuggest->pwfxDst->nChannels)
            {
                DPF("Destination channel count not %lu", pSuggest->pwfxSrc->nChannels);
                return ACMERR_NOTPOSSIBLE;
            }
        }

        //
        // If the destination sampling rate was specified, make sure
        // it's the same as the source.
        //

        if(pSuggest->fdwSuggest & ACM_FORMATSUGGESTF_NSAMPLESPERSEC)
        {
            if(pSuggest->pwfxSrc->nSamplesPerSec != pSuggest->pwfxDst->nSamplesPerSec)
            {
                DPF("Destination sampling rate not %lu", pSuggest->pwfxSrc->nSamplesPerSec);
                return ACMERR_NOTPOSSIBLE;
            }
        }

        //
        // If the destination bit resolution was specified, make sure it's
        // correct.
        //

        if(pSuggest->fdwSuggest & ACM_FORMATSUGGESTF_WBITSPERSAMPLE)
        {
            if(IMAADPCM_BITS_PER_SAMPLE != pSuggest->pwfxDst->wBitsPerSample)
            {
                DPF("Destination bit resolution not %lu", IMAADPCM_BITS_PER_SAMPLE);
                return ACMERR_NOTPOSSIBLE;
            }
        }

        //
        // Fill in the format
        //

        DPF("Suggesting ADPCM format");

        CreateAdpcmFormat(pSuggest->pwfxSrc->nChannels, pSuggest->pwfxSrc->nSamplesPerSec, &wfxAdpcm);
        CopyMemory(pSuggest->pwfxDst, &wfxAdpcm, min(pSuggest->cbwfxDst, sizeof(wfxAdpcm)));

    }
    else if(WAVE_FORMAT_XBOX_ADPCM == pSuggest->pwfxSrc->wFormatTag)
    {
        if(pSuggest->cbwfxSrc < sizeof(IMAADPCMWAVEFORMAT))
        {
            DPF("Format buffer too small for ADPCM");
            return ACMERR_NOTPOSSIBLE;
        }

        if(pSuggest->cbwfxDst < sizeof(PCMWAVEFORMAT))
        {
            DPF("Format buffer too small for PCM");
            return ACMERR_NOTPOSSIBLE;
        }

        //
        // Verify that the format is valid for our purposes
        //

        if(!IsValidAdpcmFormat((LPCIMAADPCMWAVEFORMAT)pSuggest->pwfxSrc))
        {
            DPF("Bad ADPCM format");
            return ACMERR_NOTPOSSIBLE;
        }

        //
        // If the destination format tag is specified, make sure it's
        // PCM.
        //

        if(pSuggest->fdwSuggest & ACM_FORMATSUGGESTF_WFORMATTAG)
        {
            if(WAVE_FORMAT_PCM != pSuggest->pwfxDst->wFormatTag)
            {
                DPF("Destination format tag not PCM");
                return ACMERR_NOTPOSSIBLE;
            }
        }

        //
        // If the destination channel count was specified, make sure
        // it's the same as the source.
        //

        if(pSuggest->fdwSuggest & ACM_FORMATSUGGESTF_NCHANNELS)
        {
            if(pSuggest->pwfxSrc->nChannels != pSuggest->pwfxDst->nChannels)
            {
                DPF("Destination channel count not %lu", pSuggest->pwfxSrc->nChannels);
                return ACMERR_NOTPOSSIBLE;
            }
        }

        //
        // If the destination sampling rate was specified, make sure
        // it's the same as the source.
        //

        if(pSuggest->fdwSuggest & ACM_FORMATSUGGESTF_NSAMPLESPERSEC)
        {
            if(pSuggest->pwfxSrc->nSamplesPerSec != pSuggest->pwfxDst->nSamplesPerSec)
            {
                DPF("Destination sampling rate not %lu", pSuggest->pwfxSrc->nSamplesPerSec);
                return ACMERR_NOTPOSSIBLE;
            }
        }

        //
        // If the destination bit resolution was specified, make sure it's
        // correct.
        //

        if(pSuggest->fdwSuggest & ACM_FORMATSUGGESTF_WBITSPERSAMPLE)
        {
            if(IMAADPCM_PCM_BITS_PER_SAMPLE != pSuggest->pwfxDst->wBitsPerSample)
            {
                DPF("Destination bit resolution not %lu", IMAADPCM_PCM_BITS_PER_SAMPLE);
                return ACMERR_NOTPOSSIBLE;
            }
        }

        //
        // Fill in the format
        //

        DPF("Suggesting PCM format");

        CreatePcmFormat(pSuggest->pwfxSrc->nChannels, pSuggest->pwfxSrc->nSamplesPerSec, pSuggest->pwfxDst);
    }
    else
    {
        // 
        // Bad format
        //

        DPF("Bad format tag");
        return ACMERR_NOTPOSSIBLE;
    }

    return MMSYSERR_NOERROR;
}


/****************************************************************************
 *
 *  OpenStream
 *
 *  Description:
 *      Opens a conversion stream.
 *
 *  Arguments:
 *      LPACMDRVSTREAMINSTANCE [in/out]: stream data.
 *
 *  Returns:  
 *      LONG: MMSYSTEM error code.
 *
 ****************************************************************************/

LONG
CXboxAdpcmDriver::OpenStream
(
    HDRVR                   hdrvr,
    LPACMDRVSTREAMINSTANCE  pStream
)
{
    LPCIMAADPCMWAVEFORMAT   pwfxEncode;
    LPCWAVEFORMATEX         pwfxDecode;
    BOOL                    fEncoder;
    CImaAdpcmCodec *        pCodec;

    DPF("OpenStream");

    //
    // Save pointers to the conversion formats
    //

    if(WAVE_FORMAT_PCM == pStream->pwfxSrc->wFormatTag)
    {
        if(WAVE_FORMAT_XBOX_ADPCM == pStream->pwfxDst->wFormatTag)
        {
            pwfxEncode = (LPCIMAADPCMWAVEFORMAT)pStream->pwfxDst;
            pwfxDecode = pStream->pwfxSrc;
        }
        else
        {
            DPF("Bad destination format tag");
            return ACMERR_NOTPOSSIBLE;
        }

        fEncoder = TRUE;
    }
    else if(WAVE_FORMAT_XBOX_ADPCM == pStream->pwfxSrc->wFormatTag)
    {
        if(WAVE_FORMAT_PCM == pStream->pwfxDst->wFormatTag)
        {
            pwfxEncode = (LPCIMAADPCMWAVEFORMAT)pStream->pwfxSrc;
            pwfxDecode = pStream->pwfxDst;
        }
        else
        {
            DPF("Bad destination format tag");
            return ACMERR_NOTPOSSIBLE;
        }

        fEncoder = FALSE;
    }
    else
    {
        DPF("Bad source format tag");
        return ACMERR_NOTPOSSIBLE;
    }
    
    if(!IsValidAdpcmFormat(pwfxEncode))
    {
        DPF("Bad ADPCM format");
        return ACMERR_NOTPOSSIBLE;
    }

    if(!IsValidPcmFormat(pwfxDecode))
    {
        DPF("Bad PCM format");
        return ACMERR_NOTPOSSIBLE;
    }

    if(pwfxEncode->wfx.nChannels != pwfxDecode->nChannels)
    {
        DPF("Channel-count mismatch");
        return ACMERR_NOTPOSSIBLE;
    }

    if(pwfxEncode->wfx.nSamplesPerSec != pwfxDecode->nSamplesPerSec)
    {
        DPF("Sampling-rate mismatch");
        return ACMERR_NOTPOSSIBLE;
    }

    //
    // Check for an open query as opposed to a full-on open operation
    //

    if(pStream->fdwOpen & ACM_STREAMOPENF_QUERY)
    {
        return MMSYSERR_NOERROR;
    }

    //
    // Create the CODEC object
    //

    if(!(pCodec = new CImaAdpcmCodec))
    {
        return MMSYSERR_NOMEM;
    }

    if(!pCodec->Initialize(pwfxEncode, fEncoder))
    {
        DPF("Failed to initialize CODEC");
        delete pCodec;
        return ACMERR_NOTPOSSIBLE;
    }

    //
    // Fill in stream instance data
    //

    pStream->fdwDriver = 0;
    pStream->dwDriver = (DWORD)pCodec;

    return MMSYSERR_NOERROR;
}


/****************************************************************************
 *
 *  CloseStream
 *
 *  Description:
 *      Closes an open conversion stream.
 *
 *  Arguments:
 *      LPACMDRVSTREAMINSTANCE [in/out]: stream data.
 *
 *  Returns:  
 *      LONG: MMSYSTEM error code.
 *
 ****************************************************************************/

LONG
CXboxAdpcmDriver::CloseStream
(
    HDRVR                   hdrvr,
    LPACMDRVSTREAMINSTANCE  pStream
)
{
    CImaAdpcmCodec *        pCodec  = (CImaAdpcmCodec *)pStream->dwDriver;

    DPF("CloseStream");

    if(pCodec)
    {
        delete pCodec;
    }

    return MMSYSERR_NOERROR;
}


/****************************************************************************
 *
 *  GetStreamSize
 *
 *  Description:
 *      Gets the expected size of a conversion operation.
 *
 *  Arguments:
 *      LPACMDRVSTREAMINSTANCE [in/out]: stream data.
 *      LPACMDRVSTREAMSIZE [in/out]: stream size data.
 *
 *  Returns:  
 *      LONG: MMSYSTEM error code.
 *
 ****************************************************************************/

LONG
CXboxAdpcmDriver::GetStreamSize
(
    HDRVR                   hdrvr,
    LPACMDRVSTREAMINSTANCE  pStream,
    LPACMDRVSTREAMSIZE      pSize
)
{
    CImaAdpcmCodec *        pCodec  = (CImaAdpcmCodec *)pStream->dwDriver;

    DPF("GetStreamSize");

    if(!pCodec)
    {
        DPF("No CODEC pointer");
        return ACMERR_NOTPOSSIBLE;
    }

    switch(pSize->fdwSize)
    {
        case ACM_STREAMSIZEF_SOURCE:
            
            //
            // How many bytes are needed to hold the converted data?
            //

            pSize->cbDstLength = pSize->cbSrcLength / pCodec->GetSourceAlignment() * pCodec->GetDestinationAlignment();

            break;

        case ACM_STREAMSIZEF_DESTINATION:
            
            //
            // How many bytes can be converted into the given buffer?
            //

            pSize->cbSrcLength = pSize->cbDstLength / pCodec->GetDestinationAlignment() * pCodec->GetSourceAlignment();

            break;

        default:
            DPF("Bad size request");
            return MMSYSERR_NOTSUPPORTED;
    }

    return MMSYSERR_NOERROR;
}


/****************************************************************************
 *
 *  ConvertStream
 *
 *  Description:
 *      Converts stream data.
 *
 *  Arguments:
 *      LPACMDRVSTREAMINSTANCE [in/out]: stream data.
 *      LPACMDRVSTREAMHEADER [in/out]: stream conversion data.
 *
 *  Returns:  
 *      LONG: MMSYSTEM error code.
 *
 ****************************************************************************/

LONG
CXboxAdpcmDriver::ConvertStream
(
    HDRVR                   hdrvr,
    LPACMDRVSTREAMINSTANCE  pStream,
    LPACMDRVSTREAMHEADER    pConvert
)
{
    CImaAdpcmCodec *        pCodec      = (CImaAdpcmCodec *)pStream->dwDriver;
    DWORD                   dwSrcBlocks;
    DWORD                   dwDstBlocks;
    DWORD                   dwBlocks;

    DPF("ConvertStream");

    if(!pCodec)
    {
        DPF("No CODEC pointer");
        return ACMERR_NOTPOSSIBLE;
    }

    //
    // Reset the CODEC if necessary
    //

    if(pConvert->fdwConvert & ACM_STREAMCONVERTF_START)
    {
        pCodec->Reset();
    }

    //
    // Calculate the number of blocks to convert
    //

    dwSrcBlocks = pConvert->cbSrcLength / pCodec->GetSourceAlignment();
    dwDstBlocks = pConvert->cbDstLength / pCodec->GetDestinationAlignment();

    dwBlocks = min(dwSrcBlocks, dwDstBlocks);

    //
    // Convert data
    //

    if(!pCodec->Convert(pConvert->pbSrc, pConvert->pbDst, dwBlocks))
    {
        DPF("CODEC failure");
        return ACMERR_NOTPOSSIBLE;
    }

    //
    // Return the amount of data converted
    //

    pConvert->cbSrcLengthUsed = dwBlocks * pCodec->GetSourceAlignment();
    pConvert->cbDstLengthUsed = dwBlocks * pCodec->GetDestinationAlignment();

    //
    // If the caller knew the source size wasn't block-aligned, we'll be nice
    // and tell them we used the little bit of crap at the end.
    //

    if(!(pConvert->fdwConvert & ACM_STREAMCONVERTF_BLOCKALIGN))
    {
        pConvert->cbSrcLengthUsed = pConvert->cbSrcLength;
    }

    return MMSYSERR_NOERROR;
}


