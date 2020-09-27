//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992, 1994, 2000  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;
//
//  DS_LOG.cpp
//
//  Description:
//      Utilities used by tDSound
//
//  History:
//      12/11/97    MTang           Started it
//      03/29/00    DanHaff         Copied from TDSOUND's utils.cpp
//==========================================================================;
#include "globals.h"


#define FLAG_BUFFER_LENGTH 1024
//#define ASSERT assert

void Log_wFormatTag (int nLevel, WORD wFmt);
void Log_tstLof(int nLevel, CHAR * szFormat, double flNumber);

#define WARNLOGLEVEL 1

void __inline AppendFlag (int * pnPos, CHAR * szFlagBuffer, CHAR * szFlag, BOOL * pbFirst)
{
    if (pnPos == NULL)
    {
        Log(WARNLOGLEVEL, "AppendFlag: NULL POS pointer");
        return;
    }
    if (pbFirst == NULL)
    {
        Log(WARNLOGLEVEL, "AppendFlag: NULL BOOL pointer : pbFirst");
        return;
    }
    if (szFlagBuffer == NULL)
    {
        Log(WARNLOGLEVEL, "AppendFlag: NULL flag buffer");
        return;
    }
    if (strlen(szFlag)+(*pnPos) >= FLAG_BUFFER_LENGTH)
    {
       Log(WARNLOGLEVEL, "AppendFlag: Flag buffer overflow");
       return;
    }

    if (szFlag == NULL)
      return;

    if (*pbFirst)
    {
        *pbFirst = FALSE;
        (*pnPos)+= sprintf(szFlagBuffer+(*pnPos), "%s", szFlag);
    }
    else
        (*pnPos)+= sprintf(szFlagBuffer+(*pnPos), TEXT(" | %s"), szFlag);

}

#define APPEND_FLAG(flag) AppendFlag(&nPos, szFlagBuffer, flag, &bFirst)

/*
void _stdcall Log_DS3DAlgorithm (int nLog, LPGUID lpGuid)
{
    if (NULL == lpGuid)
        return;

    if (IsEqualGUID(DS3DALG_DEFAULT, *lpGuid))
        fnsLog (nLog, "guid3DAlgorithm = DS3DALG_DEFAULT");
    else if (IsEqualGUID(DS3DALG_NO_VIRTUALIZATION, *lpGuid))
        fnsLog (nLog, "guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION");
    else if (IsEqualGUID(DS3DALG_HRTF_FULL, *lpGuid))
        fnsLog (nLog, "guid3DAlgorithm = DS3DALG_HRTF_FULL");
    else if (IsEqualGUID(DS3DALG_HRTF_LIGHT, *lpGuid))
        fnsLog (nLog, "guid3DAlgorithm = DS3DALG_HRTF_LIGHT");
//  else if (IsEqualGUID(DS3DALG_ITD, *lpGuid))
//      fnsLog (nLog, "guid3DAlgorithm = DS3DALG_ITD");
    else
        fnsLog (nLog, "guid3DAlgorithm = (Unknown algorithm)");

}
*/
void _stdcall Log_DSErr (CHAR *szPrompt, HRESULT result, int nLog)
{
    CHAR szErrorString[255];

    ZeroMemory (szErrorString, sizeof (szErrorString));

    switch(result)
    {
//    case DSERR_ALLOCATED:
//        sprintf(szErrorString, "%s returned DSERR_ALLOCATED", szPrompt);
//        break;

    case DSERR_CONTROLUNAVAIL:
        sprintf(szErrorString, "%s returned DSERR_CONTROLUNAVAIL", szPrompt);
        break;

//    case DSERR_INVALIDPARAM:
//        sprintf(szErrorString, "%s returned DSERR_INVALIDPARAM", szPrompt);
//        break;

    case DSERR_INVALIDCALL:
        sprintf(szErrorString, "%s returned DSERR_INVALIDCALL", szPrompt);
        break;

    case DSERR_GENERIC:
        sprintf(szErrorString, "%s returned DSERR_GENERIC", szPrompt);
        break;

//    case DSERR_PRIOLEVELNEEDED:
//        sprintf(szErrorString, "%s returned DSERR_PRIOLEVELNEEDED", szPrompt);
//        break;

    case DSERR_OUTOFMEMORY:
        sprintf(szErrorString, "%s returned DSERR_OUTOFMEMORY", szPrompt);
        break;

//    case DSERR_BADFORMAT:
//        sprintf(szErrorString, "%s returned DSERR_BADFORMAT", szPrompt);
//        break;

    case DSERR_UNSUPPORTED:
        sprintf(szErrorString, "%s returned DSERR_UNSUPPORTED", szPrompt);
        break;

    case DSERR_NODRIVER:
        sprintf(szErrorString, "%s returned DSERR_NODRIVER", szPrompt);
        break;

//    case DSERR_ALREADYINITIALIZED:
//        sprintf(szErrorString, "%s returned DSERR_ALREADYINITIALIZED", szPrompt);
//        break;

    case DSERR_NOAGGREGATION:
        sprintf(szErrorString, "%s returned DSERR_NOAGGREGATION", szPrompt);
        break;

//    case DSERR_BUFFERLOST:
//        sprintf(szErrorString, "%s returned DSERR_BUFFERLOST", szPrompt);
//        break;

//    case DSERR_OTHERAPPHASPRIO:
//        sprintf(szErrorString, "%s returned DSERR_OTHERAPPHASPRIO", szPrompt);
//        break;

//    case DSERR_UNINITIALIZED:
//        sprintf(szErrorString, "%s returned DSERR_UNINITIALIZED", szPrompt);
//        break;

//    case DSERR_NOINTERFACE:
//        sprintf(szErrorString, "%s returned DSERR_NOINTERFACE", szPrompt);
//        break;
/*
    case DSERR_HWUNAVAIL:
        sprintf(szErrorString, "%s returned DSERR_HWUNAVAIL", szPrompt);
        break;
*/
    default:
        sprintf(szErrorString, "%s returned unknown error code (0x%08x)", szPrompt, result);
        break;
    }

    fnsLog( nLog, szErrorString );
}

void _stdcall Log_DSCooperativeLevel (int nLog, DWORD dwLevel)
{
    CHAR szTemp[64];

    strcpy(szTemp, "Cooperative Level: ");

    switch (dwLevel) {
    case DSSCL_EXCLUSIVE:
        strcat(szTemp, "DSSCL_EXCLUSIVE ");
        break;

    case DSSCL_NORMAL:
        strcat(szTemp, "DSSCL_NORMAL ");
        break;

    case DSSCL_PRIORITY:
        strcat(szTemp, "DSSCL_PRIORITY  ");
        break;

    case DSSCL_WRITEPRIMARY:
        strcat(szTemp, "DSSCL_WRITEPRIMARY ");
        break;
    }

    fnsLog(nLog, szTemp);
}

void _stdcall Log_GUID (int nLog, LPGUID lpGuid)
{
    OLECHAR     wszGuid[256];

    if (lpGuid == NULL)
    {
        fnsLog(nLog, "  NULL GUID Pointer");
        return;
    }
    StringFromGUID2(*lpGuid, wszGuid, 256);
    fnsLog(nLog, "  GUID: %ws", wszGuid);
}

void _stdcall Log_GUID (int nLog, REFGUID Guid)
{
    OLECHAR     wszGuid[256];

    StringFromGUID2(Guid, wszGuid, 256);
    fnsLog(nLog, "  GUID: %ws", wszGuid);
}
/*


void _stdcall Log_DSCaps (int nLog, LPDSCAPS lpdscaps)
{
    // BUGBUG assuming the flag string will never exceed FLAG_BUFFER_LENGTH
    CHAR   szFlagBuffer[FLAG_BUFFER_LENGTH];
    int     nPos    = 0;
    BOOL    bFirst  = TRUE;

    ZeroMemory (szFlagBuffer, sizeof(szFlagBuffer));

    if (lpdscaps == NULL)
    {
        fnsLog(nLog, "DSCAPS : NULL pointer");
        return;
    }

    fnsLog(nLog, "DSCAPS:");
    fnsLog(nLog,
            "  dwSize                   = %lu", lpdscaps->dwSize);

    nPos += sprintf(szFlagBuffer + nPos,
                    "  dwFlags                           = ");
    if (lpdscaps->dwFlags & DSCAPS_CERTIFIED)
    {
        APPEND_FLAG(" DSCAPS_CERTIFIED");
    }
    if (lpdscaps->dwFlags & DSCAPS_CONTINUOUSRATE)
    {
        APPEND_FLAG(" DSCAPS_CONTINUOUSRATE");
    }
    if (lpdscaps->dwFlags & DSCAPS_EMULDRIVER)
    {
        APPEND_FLAG(" DSCAPS_EMULDRIVER");
    }
    if (lpdscaps->dwFlags & DSCAPS_PRIMARY16BIT)
    {
        APPEND_FLAG(" DSCAPS_PRIMARY16BIT");
    }
    if (lpdscaps->dwFlags & DSCAPS_PRIMARY8BIT)
    {
        APPEND_FLAG(" DSCAPS_PRIMARY8BIT");
    }
    if (lpdscaps->dwFlags & DSCAPS_PRIMARYMONO)
    {
        APPEND_FLAG(" DSCAPS_PRIMARYMONO");
    }
    if (lpdscaps->dwFlags & DSCAPS_PRIMARYSTEREO)
    {
        APPEND_FLAG(" DSCAPS_PRIMARYSTEREO");
    }
    if (lpdscaps->dwFlags & DSCAPS_SECONDARY16BIT)
    {
        APPEND_FLAG(" DSCAPS_SECONDARY16BIT");
    }
    if (lpdscaps->dwFlags & DSCAPS_SECONDARY8BIT)
    {
        APPEND_FLAG(" DSCAPS_SECONDARY8BIT");
    }
    if (lpdscaps->dwFlags & DSCAPS_SECONDARYMONO)
    {
        APPEND_FLAG(" DSCAPS_SECONDARYMONO");
    }
    if (lpdscaps->dwFlags & DSCAPS_SECONDARYSTEREO)
    {
        APPEND_FLAG(" DSCAPS_SECONDARYSTEREO");
    }
    fnsLog(nLog, szFlagBuffer);
//    fnsLog(nLog, "  dwMinSecondarySampleRate          = %lu",
//            lpdscaps->dwMinSecondarySampleRate);
//    fnsLog(nLog, "  dwMaxSecondarySampleRate          = %lu",
//            lpdscaps->dwMaxSecondarySampleRate);
    fnsLog(nLog, "  dwPrimaryBuffers                  = %lu",
            lpdscaps->dwPrimaryBuffers);
    fnsLog(nLog, "  dwMaxHwMixingAllBuffers           = %lu",
            lpdscaps->dwMaxHwMixingAllBuffers);
    fnsLog(nLog, "  dwMaxHwMixingStaticBuffers        = %lu",
            lpdscaps->dwMaxHwMixingStaticBuffers);
    fnsLog(nLog, "  dwMaxHwMixingStreamingBuffers     = %lu",
            lpdscaps->dwMaxHwMixingStreamingBuffers);
    fnsLog(nLog, "  dwFreeHwMixingAllBuffers          = %lu",
            lpdscaps->dwFreeHwMixingAllBuffers);
    fnsLog(nLog, "  dwFreeHwMixingStaticBuffers       = %lu",
            lpdscaps->dwFreeHwMixingStaticBuffers);
    fnsLog(nLog, "  dwFreeHwMixingStreamingBuffers    = %lu",
            lpdscaps->dwFreeHwMixingStreamingBuffers);
    fnsLog(nLog, "  dwMaxHw3DAllBuffers               = %lu",
            lpdscaps->dwMaxHw3DAllBuffers);
    fnsLog(nLog, "  dwMaxHw3DStaticBuffers            = %lu",
            lpdscaps->dwMaxHw3DStaticBuffers);
    fnsLog(nLog, "  dwMaxHw3DStreamingBuffers         = %lu",
            lpdscaps->dwMaxHw3DStreamingBuffers);
    fnsLog(nLog, "  dwTotalHwMemBytes                 = %lu",
            lpdscaps->dwTotalHwMemBytes);
    fnsLog(nLog, "  dwFreeHwMemBytes                  = %lu",
            lpdscaps->dwFreeHwMemBytes);
    fnsLog(nLog, "  dwMaxContigFreeHwMemBytes         = %lu",
            lpdscaps->dwMaxContigFreeHwMemBytes);
    fnsLog(nLog, "  dwUnlockTransferRateHwBuffers     = %lu",
            lpdscaps->dwUnlockTransferRateHwBuffers);
    fnsLog(nLog, "  dwPlayCpuOverheadSwBuffers        = %lu",
            lpdscaps->dwPlayCpuOverheadSwBuffers);
    fnsLog(nLog, "  dwReserved1                       = 0x%08x",
            lpdscaps->dwReserved1);
    fnsLog(nLog, "  dwReserved2                       = 0x%08x",
            lpdscaps->dwReserved2);

}

*/


void _stdcall Log_DSBCapsFlags(int nLog, DWORD dwFlags)
{
    // BUGBUG assuming the flag string will never exceed FLAG_BUFFER_LENGTH
    CHAR   szFlagBuffer[FLAG_BUFFER_LENGTH];
    int     nPos    = 0;
    BOOL    bFirst  = TRUE;

    ZeroMemory (szFlagBuffer, sizeof (szFlagBuffer));

    nPos += sprintf(szFlagBuffer + nPos,
                    "  dwFlags               = ");
    if (dwFlags & DSBCAPS_CTRL3D)
    {
        APPEND_FLAG (" DSBCAPS_CTRL3D");
    }
//  if (dwFlags & DSBCAPS_CTRLCHANNELVOLUME)
//  {
//      APPEND_FLAG (" DSBCAPS_CTRLCHANNELVOLUME");
//  }
    if (dwFlags & DSBCAPS_CTRLFREQUENCY)
    {
        APPEND_FLAG (" DSBCAPS_CTRLFREQUENCY");
    }
    if (dwFlags & DSBCAPS_CTRLFX)
    {
        APPEND_FLAG (" DSBCAPS_CTRLFX");
    }
//    if (dwFlags & DSBCAPS_CTRLPAN)
//    {
//        APPEND_FLAG (" DSBCAPS_CTRLPAN");
//    }
    if (dwFlags & DSBCAPS_CTRLVOLUME)
    {
        APPEND_FLAG (" DSBCAPS_CTRLVOLUME");
    }
    if (dwFlags & DSBCAPS_CTRLPOSITIONNOTIFY)
    {
        APPEND_FLAG (" DSBCAPS_CTRLPOSITIONNOTIFY");
    }
//    if (dwFlags & DSBCAPS_GETCURRENTPOSITION2)
//    {
        //APPEND_FLAG (" DSBCAPS_GETCURRENTPOSITION2");
//    }
//    if (dwFlags & DSBCAPS_GLOBALFOCUS)
//    {
//        APPEND_FLAG (" DSBCAPS_GLOBALFOCUS");
//    }
    if (dwFlags & DSBCAPS_LOCDEFER)
    {
        APPEND_FLAG (" DSBCAPS_LOCDEFER");
    }
    if (dwFlags & DSBCAPS_LOCHARDWARE)
    {
        APPEND_FLAG (" DSBCAPS_LOCHARDWARE");
    }
    if (dwFlags & DSBCAPS_LOCSOFTWARE)
    {
        APPEND_FLAG (" DSBCAPS_LOCSOFTWARE");
    }

/*
    if (dwFlags & DSBCAPS_MIXIN)
    {
        APPEND_FLAG (" DSBCAPS_MIXIN");
    }
*/
//    if (dwFlags & DSBCAPS_MUTE3DATMAXDISTANCE)
//    {
//        APPEND_FLAG (" DSBCAPS_MUTE3DATMAXDISTANCE");
//    }
//    if (dwFlags & DSBCAPS_PRIMARYBUFFER)
//    {
//        APPEND_FLAG (" DSBCAPS_PRIMARYBUFFER");
//    }
//    if (dwFlags & DSBCAPS_STATIC)
//    {
//        APPEND_FLAG (" DSBCAPS_STATIC");
    //}
//    if (dwFlags & DSBCAPS_STICKYFOCUS)
//    {
//        APPEND_FLAG (" DSBCAPS_STIKYFOCUS");
//    }

    fnsLog(nLog, szFlagBuffer);
}


/*
void _stdcall Log_DSBCaps (int nLog, LPDSBCAPS lpdsbcaps)
{
    if (lpdsbcaps == NULL)
    {
        fnsLog(nLog, "DSBCAPS : NULL pointer");
        return;
    }

    fnsLog(nLog, "DSBCAPS :");
    fnsLog(nLog,
            "  dwSize                            = %lu", lpdsbcaps->dwSize);
    Log_DSBCapsFlags(nLog, lpdsbcaps->dwFlags);
    fnsLog(nLog, "  dwBufferBytes           = %lu", lpdsbcaps->dwBufferBytes);
    fnsLog(nLog, "  dwUnlockTransferRate    = %lu", lpdsbcaps->dwUnlockTransferRate);
    fnsLog(nLog, "  dwPlayCpuOverhead       = %lu", lpdsbcaps->dwPlayCpuOverhead);
}

void _stdcall Log_DSCCaps (int nLog, LPDSCCAPS lpdsccaps)
{
    // BUGBUG assuming the flag string will never exceed FLAG_BUFFER_LENGTH
    CHAR   szFlagBuffer[FLAG_BUFFER_LENGTH];
    int     nPos    = 0;
    BOOL    bFirst  = TRUE;

    ZeroMemory (szFlagBuffer, sizeof(szFlagBuffer));

    if (lpdsccaps == NULL)
    {
        fnsLog(nLog, "DSCCAPS : NULL pointer");
        return;
    }

    fnsLog(nLog, "DSCCAPS :");
    fnsLog(nLog,
        "  dwSize                            = %lu", lpdsccaps->dwSize);

    nPos += sprintf(szFlagBuffer + nPos,
                    "  dwFlags                           = ");
    if (lpdsccaps->dwFlags & DSCCAPS_EMULDRIVER)
    {
        APPEND_FLAG(" DSCCAPS_EMULDRIVER");
    }

    fnsLog(nLog, szFlagBuffer);
    fnsLog(nLog, "  dwFormats    = 0x%08x", lpdsccaps->dwFormats);
    fnsLog(nLog, "  dwChannels   = 0x%08x", lpdsccaps->dwChannels);
}

void _stdcall Log_DSCBCapsFlags(int nLog, DWORD dwFlags)
{
    // BUGBUG assuming the flag string will never exceed FLAG_BUFFER_LENGTH
    CHAR   szFlagBuffer[FLAG_BUFFER_LENGTH];
    int     nPos    = 0;
    BOOL    bFirst  = TRUE;


    ZeroMemory (szFlagBuffer, sizeof(szFlagBuffer));

    nPos += sprintf(szFlagBuffer + nPos,
                    "  dwFlags                           = ");
    if (dwFlags & DSCBCAPS_WAVEMAPPED)
    {
        APPEND_FLAG (" DSCBCAPS_WAVEMAPPED");
    }

    fnsLog(nLog, szFlagBuffer);
}

void _stdcall Log_DSCBCaps (int nLog, LPDSCBCAPS lpdscbcaps)
{
    if (lpdscbcaps == NULL)
    {
        fnsLog(nLog, "DSCBCAPS : NULL pointer");
        return;
    }

    fnsLog(nLog, "DSCBCAPS :");
    fnsLog(nLog,
            "  dwSize                            = %lu", lpdscbcaps->dwSize);
    Log_DSCBCapsFlags(nLog, lpdscbcaps->dwFlags);
    fnsLog(nLog, "  dwBufferBytes    = %lu", lpdscbcaps->dwBufferBytes);
    fnsLog(nLog, "  dwReserved       = 0x%08x", lpdscbcaps->dwReserved);
}
*/
void _stdcall Log_SpeakerConfig (int nLog, DWORD dwSpkrCfg)
{
    CHAR szConfiguration[255];

    ZeroMemory (szConfiguration, sizeof(szConfiguration));

    fnsLog(nLog, "Speaker configuration:");

    if (DSSPEAKER_CONFIG(dwSpkrCfg) == DSSPEAKER_HEADPHONE)
    {
        sprintf(szConfiguration, "  DSSPEAKER_HEADPHONE");
    }
    else if (DSSPEAKER_CONFIG(dwSpkrCfg) == DSSPEAKER_MONO)
    {
        sprintf(szConfiguration, "  DSSPEAKER_MONO");
    }
    else if (DSSPEAKER_CONFIG(dwSpkrCfg) == DSSPEAKER_QUAD)
    {
        sprintf(szConfiguration, "  DSSPEAKER_QUAD");
    }
    else if (DSSPEAKER_CONFIG(dwSpkrCfg) == DSSPEAKER_SURROUND)
    {
        sprintf(szConfiguration, "  DSSPEAKER_SURROUND");
    }
    else if (DSSPEAKER_CONFIG(dwSpkrCfg) == DSSPEAKER_STEREO)
    {
        if (DSSPEAKER_GEOMETRY(dwSpkrCfg) == DSSPEAKER_GEOMETRY_WIDE)
            sprintf(szConfiguration, "  DSSPEAKER_STEREO | DSSPEAKER_GEOMETRY_WIDE");
        else if (DSSPEAKER_GEOMETRY(dwSpkrCfg) == DSSPEAKER_GEOMETRY_NARROW)
            sprintf(szConfiguration, "  DSSPEAKER_STEREO | DSSPEAKER_GEOMETRY_NARROW");
        else if (DSSPEAKER_GEOMETRY(dwSpkrCfg) == DSSPEAKER_GEOMETRY_MIN)
            sprintf(szConfiguration, "  DSSPEAKER_STEREO | DSSPEAKER_GEOMETRY_MIN");
        else if (DSSPEAKER_GEOMETRY(dwSpkrCfg) == DSSPEAKER_GEOMETRY_MAX)
            sprintf(szConfiguration, "  DSSPEAKER_STEREO | DSSPEAKER_GEOMETRY_MAX");
        else
            sprintf(szConfiguration, "  DSSPEAKER_STEREO");
    }
    else if (DSSPEAKER_CONFIG(dwSpkrCfg) == DSSPEAKER_5POINT1)
    {
        sprintf(szConfiguration, "  DSSPEAKER_5POINT1");
    }
    else
    {
        sprintf(szConfiguration, "  Unknown configuration");
    }

    fnsLog(nLog, szConfiguration);
}



/********************************************************************************
HISTORY:
    danhaff         01/11/00        Made aware of DRM wave formats.
********************************************************************************/
void _stdcall Log_WaveFormatEx(int nLevel, LPCWAVEFORMATEX lpwfx)
{
    if(!lpwfx)
    {
        fnsLog(nLevel, "LPWAVEFORMATEX:  0x%p", lpwfx);
    }
    else
    {
        Log_wFormatTag(nLevel, lpwfx->wFormatTag);

        //Should we log the original WFX in this case?  I am not, for now.
        if (WAVE_FORMAT_DRM == lpwfx->wFormatTag)
        {
            DRMWAVEFORMAT * p = (DRMWAVEFORMAT *)lpwfx;
            fnsLog(nLevel, "WFX                           ");
            fnsLog(nLevel, "  nChannels:                  %d", p->wfx.nChannels);
            fnsLog(nLevel, "  nSamplesPerSec:             %d", p->wfx.nSamplesPerSec);
            fnsLog(nLevel, "  nAvgBytesPerSec:            %d", p->wfx.nAvgBytesPerSec);
            fnsLog(nLevel, "  nBlockAlign:                %d", p->wfx.nBlockAlign);
            fnsLog(nLevel, "  wBitsPerSample:             %d", p->wfx.wBitsPerSample);
            fnsLog(nLevel, "  cbSize:                     %d", p->wfx.cbSize);
            fnsLog(nLevel, "");
            fnsLog(nLevel, "  wReserved:                  %d", p->wReserved);
            fnsLog(nLevel, "  ulContentId:                %d", p->ulContentId);
            fnsLog(nLevel, "WFXSECURE                     ");
            Log_WaveFormatEx(nLevel, &p->wfxSecure);
        }
        else if (WAVE_FORMAT_EXTENSIBLE == lpwfx->wFormatTag)
        {
            WAVEFORMATEXTENSIBLE *  p = (WAVEFORMATEXTENSIBLE *)lpwfx;
            fnsLog(nLevel, "  nChannels:                  %d", p->Format.nChannels);
            fnsLog(nLevel, "  nSamplesPerSec:             %d", p->Format.nSamplesPerSec);
            fnsLog(nLevel, "  nAvgBytesPerSec:            %d", p->Format.nAvgBytesPerSec);
            fnsLog(nLevel, "  nBlockAlign:                %d", p->Format.nBlockAlign);
            fnsLog(nLevel, "  wBitsPerSample:             %d", p->Format.wBitsPerSample);
            fnsLog(nLevel, "  cbSize:                     %d", p->Format.cbSize);

            fnsLog(nLevel, "  wValidBitsPerSample:        %d", p->Samples.wValidBitsPerSample);
            fnsLog(nLevel, "  dwChannelMask:              0x%08x", p->dwChannelMask);
            if (IsEqualGUID(p->SubFormat, KSDATAFORMAT_SUBTYPE_PCM))
            {
                fnsLog (nLevel, "  SubFormat:                  KSDATAFORMAT_SUBTYPE_PCM");
            }
            else if (IsEqualGUID(p->SubFormat, KSDATAFORMAT_SUBTYPE_ADPCM))
            {
                fnsLog (nLevel, "  SubFormat:                  KSDATAFORMAT_SUBTYPE_ADPCM");
            }
            else if (IsEqualGUID(p->SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
            {
                fnsLog (nLevel, "  SubFormat:                  KSDATAFORMAT_SUBTYPE_IEEE_FLOAT");
            }
            else
            {
                fnsLog (nLevel, "  SubFormat:                  UNKNOWN");
                Log_GUID (nLevel, p->SubFormat);
            }
        }
        else //go with default PCM-style data.
        {
            fnsLog(nLevel, "  nChannels:        %d", lpwfx->nChannels);
            fnsLog(nLevel, "  nSamplesPerSec:   %d", lpwfx->nSamplesPerSec);
            fnsLog(nLevel, "  nAvgBytesPerSec:  %d", lpwfx->nAvgBytesPerSec);
            fnsLog(nLevel, "  nBlockAlign:      %d", lpwfx->nBlockAlign);
            fnsLog(nLevel, "  wBitsPerSample:   %d", lpwfx->wBitsPerSample);

            //Can't depend on cbSize being valid if we're only PCM.
            if (WAVE_FORMAT_PCM != lpwfx->wFormatTag)
                fnsLog(nLevel, "  cbSize        :   %d", lpwfx->cbSize);
        }

    }
}

void _stdcall Log_wFormatTag(int nLevel, WORD wFmt)
{
    CHAR    szFormat[FLAG_BUFFER_LENGTH] = " ";

    ZeroMemory (szFormat, sizeof(szFormat));

    fnsLog(nLevel, "wFormatTag:       0x%04x", wFmt);

    Log(nLevel, "wFormatTag:       0x%04x", wFmt);

    switch(wFmt)
    {
        case WAVE_FORMAT_DRM:
            sprintf(szFormat, "WAVE_FORMAT_DRM");
            break;

        case WAVE_FORMAT_EXTENSIBLE:
            sprintf(szFormat, "WAVE_FORMAT_EXTENSIBLE");
            break;

        case WAVE_FORMAT_PCM:
            sprintf(szFormat, "WAVE_FORMAT_PCM");
            break;

        case WAVE_FORMAT_ADPCM:
            sprintf (szFormat, "WAVE_FORMAT_ADPCM");
            break;

        case WAVE_FORMAT_IBM_CVSD   :
            sprintf (szFormat, "WAVE_FORMAT_IBM_CVSD   ");
            break;

        case WAVE_FORMAT_ALAW       :
            sprintf (szFormat, "WAVE_FORMAT_ALAW       ");
            break;

        case WAVE_FORMAT_MULAW      :
            sprintf (szFormat, "WAVE_FORMAT_MULAW      ");
            break;

        case WAVE_FORMAT_OKI_ADPCM  :
            sprintf (szFormat, "WAVE_FORMAT_OKI_ADPCM  ");
            break;

        case WAVE_FORMAT_IMA_ADPCM  :
            sprintf (szFormat, "WAVE_FORMAT_IMA_ADPCM  ");
            break;

        case WAVE_FORMAT_MEDIASPACE_ADPCM:
            sprintf (szFormat, "WAVE_FORMAT_MEDIASPACE_ADPCM");
            break;

        case WAVE_FORMAT_SIERRA_ADPCM:
            sprintf (szFormat, "WAVE_FORMAT_SIERRA_ADPCM");
            break;

        case WAVE_FORMAT_G723_ADPCM :
            sprintf (szFormat, "WAVE_FORMAT_G723_ADPCM ");
            break;

        case WAVE_FORMAT_DIGISTD    :
            sprintf (szFormat, "WAVE_FORMAT_DIGISTD    ");
            break;

        case WAVE_FORMAT_DIGIFIX    :
            sprintf (szFormat, "WAVE_FORMAT_DIGIFIX    ");
            break;

        case WAVE_FORMAT_DIALOGIC_OKI_ADPCM :
            sprintf (szFormat, "WAVE_FORMAT_DIALOGIC_OKI_ADPCM ");
            break;

        case WAVE_FORMAT_YAMAHA_ADPCM       :
            sprintf (szFormat, "WAVE_FORMAT_YAMAHA_ADPCM       ");
            break;

        case WAVE_FORMAT_SONARC :
            sprintf (szFormat, "WAVE_FORMAT_SONARC ");
            break;

        case WAVE_FORMAT_DSPGROUP_TRUESPEECH  :
            sprintf (szFormat, "WAVE_FORMAT_DSPGROUP_TRUESPEECH  ");
            break;

        case WAVE_FORMAT_ECHOSC1 :
            sprintf (szFormat, "WAVE_FORMAT_ECHOSC1 ");
            break;

        case WAVE_FORMAT_AUDIOFILE_AF36:
            sprintf (szFormat, "WAVE_FORMAT_AUDIOFILE_AF36");
            break;

        case WAVE_FORMAT_APTX :
            sprintf (szFormat, "WAVE_FORMAT_APTX ");
            break;

        case WAVE_FORMAT_AUDIOFILE_AF10    :
            sprintf (szFormat, "WAVE_FORMAT_AUDIOFILE_AF10    ");
            break;

        case WAVE_FORMAT_DOLBY_AC2 :
            sprintf (szFormat, "WAVE_FORMAT_DOLBY_AC2 ");
            break;

        case WAVE_FORMAT_GSM610    :
            sprintf (szFormat, "WAVE_FORMAT_GSM610    ");
            break;

        case WAVE_FORMAT_ANTEX_ADPCME      :
            sprintf (szFormat, "WAVE_FORMAT_ANTEX_ADPCME      ");
            break;

        case WAVE_FORMAT_CONTROL_RES_VQLPC :
            sprintf (szFormat, "WAVE_FORMAT_CONTROL_RES_VQLPC ");
            break;

        case WAVE_FORMAT_DIGIREAL   :
            sprintf (szFormat, "WAVE_FORMAT_DIGIREAL   ");
            break;

        case WAVE_FORMAT_DIGIADPCM  :
            sprintf (szFormat, "WAVE_FORMAT_DIGIADPCM  ");
            break;

        case WAVE_FORMAT_CONTROL_RES_CR10  :
            sprintf (szFormat, "WAVE_FORMAT_CONTROL_RES_CR10  ");
            break;

        case WAVE_FORMAT_NMS_VBXADPCM  :
            sprintf (szFormat, "WAVE_FORMAT_NMS_VBXADPCM  ");
            break;

        case WAVE_FORMAT_CS_IMAADPCM :
            sprintf (szFormat, "WAVE_FORMAT_CS_IMAADPCM ");
            break;

        case WAVE_FORMAT_G721_ADPCM :
            sprintf (szFormat, "WAVE_FORMAT_G721_ADPCM ");
            break;

        case WAVE_FORMAT_MPEG       :
            sprintf (szFormat, "WAVE_FORMAT_MPEG       ");
            break;

        case WAVE_FORMAT_CREATIVE_ADPCM :
            sprintf (szFormat, "WAVE_FORMAT_CREATIVE_ADPCM ");
            break;

        case WAVE_FORMAT_CREATIVE_FASTSPEECH8 :
            sprintf (szFormat, "WAVE_FORMAT_CREATIVE_FASTSPEECH8 ");
            break;

        case WAVE_FORMAT_CREATIVE_FASTSPEECH10:
            sprintf (szFormat, "WAVE_FORMAT_CREATIVE_FASTSPEECH10");
            break;

        case WAVE_FORMAT_FM_TOWNS_SND :
            sprintf (szFormat, "WAVE_FORMAT_FM_TOWNS_SND ");
            break;

        case WAVE_FORMAT_OLIGSM   :
            sprintf (szFormat, "WAVE_FORMAT_OLIGSM   ");
            break;

        case WAVE_FORMAT_OLIADPCM :
            sprintf (szFormat, "WAVE_FORMAT_OLIADPCM ");
            break;

        case WAVE_FORMAT_OLICELP  :
            sprintf (szFormat, "WAVE_FORMAT_OLICELP");
            break;

        case WAVE_FORMAT_OLISBC   :
            sprintf (szFormat, "WAVE_FORMAT_OLISBC");
            break;

        case WAVE_FORMAT_OLIOPR   :
            sprintf (szFormat, "WAVE_FORMAT_OLIOPR");
            break;

        case WAVE_FORMAT_DEVELOPMENT:
            sprintf (szFormat, "WAVE_FORMAT_DEVELOPMENT");
            break;

        default :
            sprintf (szFormat, "Unknown Wave Format");
     }

     fnsLog(nLevel, "  wFormatTag:       %s", szFormat);
}

void _stdcall Log_DSBufferDesc(int nLevel, LPCDSBUFFERDESC lpdsbd)
{
    if (NULL == lpdsbd)
        return;

    // for the compatibility with old DSBUFFERDESC before dx7
    if (!IsBadReadPtr(lpdsbd, lpdsbd->dwSize))
    {
        fnsLog(nLevel, "  dwSize: %lu", lpdsbd->dwSize);
        Log_DSBCapsFlags(nLevel, lpdsbd->dwFlags);
        fnsLog(nLevel, "  dwBufferBytes:  %lu", lpdsbd->dwBufferBytes);
        fnsLog(nLevel, "  dwReserved:  %lu", lpdsbd->dwReserved);
        Log_WaveFormatEx(nLevel, lpdsbd->lpwfxFormat);
        Log_DS3DAlgorithm(nLevel, (LPGUID) &(lpdsbd->guid3DAlgorithm));
    }
}

void _stdcall Log_DSCBufferDesc (int nLevel, LPCDSCBUFFERDESC lpdscbd)
{
    if (NULL == lpdscbd)
        return;

//    if (!IsBadReadPtr(lpdscbd, sizeof(DSCBUFFERDESC)))
    // for the compatibility with old DSCBUFFERDESC before dx7
    if (!IsBadReadPtr(lpdscbd, lpdscbd->dwSize))
    {
        fnsLog(nLevel, "  dwSize:           %lu", lpdscbd->dwSize);
        Log_DSCBCapsFlags(nLevel, lpdscbd->dwFlags);
        fnsLog(nLevel, "  dwBufferBytes:    %lu", lpdscbd->dwBufferBytes);
        fnsLog(nLevel, "  dwReserved:       %lu", lpdscbd->dwReserved);
        fnsLog(nLevel, "  LPWAVEFORMATEX: 0x%p", lpdscbd->lpwfxFormat);
        Log_WaveFormatEx(nLevel, lpdscbd->lpwfxFormat);
    }
}

void _stdcall Log_DSBLockFlags(int nLog, DWORD dwFlags)
{
    CHAR szFlags[255];

    ZeroMemory (szFlags, sizeof(szFlags));

    fnsLog(nLog, "Lock flags:");

    if (dwFlags & DSBLOCK_FROMWRITECURSOR)
        sprintf(szFlags, "  DSBLOCK_FROMWRITECURSOR");
    else if (dwFlags & DSBLOCK_ENTIREBUFFER)
        sprintf(szFlags, "  DSBLOCK_ENTIREBUFFER");

    fnsLog(nLog, szFlags);
}

void _stdcall Log_DSCBLockFlags(int nLog, DWORD dwFlags)
{
    CHAR szFlags[255];

    ZeroMemory (szFlags, sizeof(szFlags));

    fnsLog(nLog, "Lock flags:");

    if (dwFlags & DSCBLOCK_ENTIREBUFFER)
        sprintf(szFlags, "  DSCBLOCK_ENTIREBUFFER");

    fnsLog(nLog, szFlags);
}

void _stdcall Log_DSBPlayFlags(int nLog, DWORD dwFlags)
{
    CHAR   szFlagBuffer[FLAG_BUFFER_LENGTH];
    int     nPos    = 0;
    BOOL    bFirst  = TRUE;

    ZeroMemory (szFlagBuffer, sizeof (szFlagBuffer));

    nPos += sprintf(szFlagBuffer + nPos,
                    "  dwFlags  = ");
    if (dwFlags & DSBPLAY_LOOPING)
    {
        APPEND_FLAG (" DSBPLAY_LOOPING");
    }


    if (dwFlags & DSBPLAY_LOCHARDWARE)
    {
        APPEND_FLAG (" DSBPLAY_LOCHARDWARE");
    }

    if (dwFlags & DSBPLAY_LOCSOFTWARE)
    {
        APPEND_FLAG (" DSBPLAY_LOCSOFTWARE");
    }

    if (dwFlags & DSBPLAY_TERMINATEBY_TIME)
    {
        APPEND_FLAG (" DSBPLAY_TERMINATEBY_TIME");
    }

    if (dwFlags & DSBPLAY_TERMINATEBY_DISTANCE)
    {
        APPEND_FLAG (" DSBPLAY_TERMINATEBY_DISTANCE");
    }

    if (dwFlags & DSBPLAY_TERMINATEBY_PRIORITY)
    {
        APPEND_FLAG (" DSBPLAY_TERMINATEBY_PRIORITY");
    }

    fnsLog(nLog, szFlagBuffer);
}

void _stdcall Log_DSCBStartFlags (int nLog, DWORD dwFlags)
{
    CHAR szFlags[255];

    ZeroMemory (szFlags, sizeof(szFlags));

    fnsLog(nLog, "Start flags:");

    if (dwFlags & DSCBSTART_LOOPING)
        sprintf(szFlags, "  DSCBSTART_LOOPING");

    fnsLog(nLog, szFlags);
}

void _stdcall Log_DSBStatus (int nLog, DWORD dwFlags)
{
    CHAR   szFlagBuffer[FLAG_BUFFER_LENGTH];
    int     nPos    = 0;
    BOOL    bFirst  = TRUE;

    ZeroMemory (szFlagBuffer, sizeof (szFlagBuffer));

    nPos += sprintf(szFlagBuffer + nPos,
                    "  dwStatus  = ");
    if (dwFlags & DSBSTATUS_PLAYING)
    {
        APPEND_FLAG (" DSBSTATUS_PLAYING");
    }

    if (dwFlags & DSBSTATUS_LOOPING)
    {
        APPEND_FLAG (" DSBSTATUS_LOOPING");
    }

    if (dwFlags & DSBSTATUS_BUFFERLOST)
    {
        APPEND_FLAG (" DSBSTATUS_BUFFERLOST");
    }

    if (dwFlags & DSBSTATUS_LOCSOFTWARE)
    {
        APPEND_FLAG (" DSBSTATUS_LOCSOFTWARE");
    }

    if (dwFlags & DSBSTATUS_LOCHARDWARE)
    {
        APPEND_FLAG (" DSBSTATUS_LOCHARDWARE");
    }
    if (dwFlags & DSBSTATUS_TERMINATED)
    {
        APPEND_FLAG (" DSBSTATUS_TERMINATED");
    }


    fnsLog(nLog, szFlagBuffer);
}

void _stdcall Log_DSCBStatus (int nLog, DWORD dwFlags)
{
    CHAR   szFlagBuffer[FLAG_BUFFER_LENGTH];
    int     nPos    = 0;
    BOOL    bFirst  = TRUE;

    ZeroMemory (szFlagBuffer, sizeof(szFlagBuffer));

    nPos += sprintf(szFlagBuffer + nPos,
                    "  dwStatus  = ");
    if (dwFlags & DSCBSTATUS_CAPTURING)
    {
        APPEND_FLAG (" DSCBSTATUS_CAPTURING");
    }
    if (dwFlags & DSCBSTATUS_LOOPING)
    {
        APPEND_FLAG (" DSCBSTATUS_LOOPING");
    }

    fnsLog(nLog, szFlagBuffer);
}

void _stdcall Log_DS3DApplyMode (int nLog, DWORD dwMode)
{
    CHAR szMode[255];

    ZeroMemory (szMode, sizeof(szMode));

    if (dwMode == DS3D_DEFERRED)
        sprintf(szMode, "  DS3D_DEFERRED");
    else if (dwMode == DS3D_IMMEDIATE)
        sprintf(szMode, "  DS3D_IMMEDIATE");

    fnsLog(nLog, szMode);
}

void _stdcall Log_DS3DBMode (int nLog, DWORD dwMode)
{
    CHAR szMode[255];

    ZeroMemory (szMode, sizeof(szMode));

    switch (dwMode)
    {
    case DS3DMODE_DISABLE:
        sprintf(szMode, "  DS3DMODE_DISABLE");
        break;

    case DS3DMODE_HEADRELATIVE:
        sprintf(szMode, "  DS3DMODE_HEADRELATIVE");
        break;

    case DS3DMODE_NORMAL:
        sprintf(szMode, "  DS3DMODE_NORMAL");
        break;

    default:
        sprintf(szMode, "  Unknown mode");
        break;
    }

    fnsLog(nLog, szMode);
}

void _stdcall Log_D3DVector(int nLog, const struct _D3DVECTOR * lpv)
{
    if (lpv == NULL)
        return;

    tstLof(nLog, "    %19.9g", lpv->x);
    tstLof(nLog, "    %19.9g", lpv->y);
    tstLof(nLog, "    %19.9g", lpv->z);
}

void _stdcall Log_DS3DLAllParameters(int nLog, LPCDS3DLISTENER lpl)
{
    if (lpl == NULL)
        return;

    fnsLog(nLog, "  dwSize               = %lu", lpl->dwSize);
    fnsLog(nLog, "  vPosition:");
    Log_D3DVector(nLog, &(lpl->vPosition));
    fnsLog(nLog, "  vVelocity:");
    Log_D3DVector(nLog, &(lpl->vVelocity));
    fnsLog(nLog, "  vOrientFront:");
    Log_D3DVector(nLog, &(lpl->vOrientFront));
    fnsLog(nLog, "  vOrientTop:");
    Log_D3DVector(nLog, &(lpl->vOrientTop));

    tstLof(nLog,  "  flDistanceFactor     = %19.9g", lpl->flDistanceFactor);
    tstLof(nLog,  "  flRolloffFactor      = %19.9g", lpl->flRolloffFactor);
    tstLof(nLog,  "  flDopplerFactor      = %19.9g", lpl->flDopplerFactor);
}

void _stdcall Log_DS3DBAllParameters(int nLog, LPCDS3DBUFFER lpb)
{
    if (lpb == NULL)
        return;

    fnsLog(nLog, "  dwSize               = %lu", lpb->dwSize);
    fnsLog(nLog, "  vPosition:");
    Log_D3DVector(nLog, &(lpb->vPosition));
    fnsLog(nLog, "  vVelocity:");
    Log_D3DVector(nLog, &(lpb->vVelocity));
    fnsLog(nLog, "  dwInsideConeAngle    = %lu", lpb->dwInsideConeAngle);
    fnsLog(nLog, "  dwOutsideConeAngle   = %lu", lpb->dwOutsideConeAngle);
    fnsLog(nLog, "  vConeOrientation:");
    Log_D3DVector(nLog, &(lpb->vConeOrientation));
    fnsLog(nLog, "  lConeOutsideVolume   = %ld", lpb->lConeOutsideVolume);

    tstLof(nLog,  "  flMinDistance        = %19.9g", lpb->flMinDistance);
    tstLof(nLog,  "  flMaxDistance        = %19.9g", lpb->flMaxDistance);

    fnsLog(nLog, "  dwMode :");
    Log_DS3DBMode(nLog, lpb->dwMode);
}

void _stdcall Log_DSNPositionNotifies (int nLog, DWORD dwNum, LPCDSBPOSITIONNOTIFY notifies)
{
    ULONG i;

    if (notifies == NULL)
        return;

    fnsLog(nLog, "  %lu notification positions:", dwNum);

    for(i=0;i<(ULONG) dwNum;i++)
    {
        fnsLog(nLog, "  Notification %lu:", i);
        fnsLog(nLog, "      dwOffset      =       0x%08x", notifies[i].dwOffset);
        fnsLog(nLog, "      hEventNotify  =       0x%p", notifies[i].hEventNotify);
    }
}

void tstLof(int nLevel, CHAR * szFormat, double flNumber)
{
    //BUGBUG, assuming the printout never exceeds 255 characters
    CHAR szBuffer[256];
    assert(szFormat);
    _stprintf(szBuffer, szFormat, flNumber);
    fnsLog(nLevel, szBuffer);
}

void _stdcall Log_DS3DAlgorithm (int nLog, LPGUID lpGuid)
{

    if (NULL == lpGuid)
        return;

    if (IsEqualGUID(DS3DALG_DEFAULT, *lpGuid))
        Log (nLog, "guid3DAlgorithm = DS3DALG_DEFAULT");
    else if (IsEqualGUID(DS3DALG_NO_VIRTUALIZATION, *lpGuid))
        Log (nLog, "guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION");
    else if (IsEqualGUID(DS3DALG_HRTF_FULL, *lpGuid))
        Log (nLog, "guid3DAlgorithm = DS3DALG_HRTF_FULL");
    else if (IsEqualGUID(DS3DALG_HRTF_LIGHT, *lpGuid))
        Log (nLog, "guid3DAlgorithm = DS3DALG_HRTF_LIGHT");
    else
        Log (nLog, "guid3DAlgorithm = (Unknown algorithm)");
}

void _stdcall Log_DSErr (CHAR *szPrompt, HRESULT result, int nLog)
{
    CHAR szErrorString[255];

    switch(result)
    {
    case DSERR_ALLOCATED:
        sprintf(szErrorString, "%s returned DSERR_ALLOCATED", szPrompt);
        break;

    case DSERR_CONTROLUNAVAIL:
        sprintf(szErrorString, "%s returned DSERR_CONTROLUNAVAIL", szPrompt);
        break;

    case DSERR_INVALIDPARAM:
        sprintf(szErrorString, "%s returned DSERR_INVALIDPARAM", szPrompt);
        break;

    case DSERR_INVALIDCALL:
        sprintf(szErrorString, "%s returned DSERR_INVALIDCALL", szPrompt);
        break;

    case DSERR_GENERIC:
        sprintf(szErrorString, "%s returned DSERR_GENERIC", szPrompt);
        break;

    case DSERR_PRIOLEVELNEEDED:
        sprintf(szErrorString, "%s returned DSERR_PRIOLEVELNEEDED", szPrompt);
        break;

    case DSERR_OUTOFMEMORY:
        sprintf(szErrorString, "%s returned DSERR_OUTOFMEMORY", szPrompt);
        break;

    case DSERR_BADFORMAT:
        sprintf(szErrorString, "%s returned DSERR_BADFORMAT", szPrompt);
        break;

    case DSERR_UNSUPPORTED:
        sprintf(szErrorString, "%s returned DSERR_UNSUPPORTED", szPrompt);
        break;

    case DSERR_NODRIVER:
        sprintf(szErrorString, "%s returned DSERR_NODRIVER", szPrompt);
        break;

    case DSERR_ALREADYINITIALIZED:
        sprintf(szErrorString, "%s returned DSERR_ALREADYINITIALIZED", szPrompt);
        break;

    case DSERR_NOAGGREGATION:
        sprintf(szErrorString, "%s returned DSERR_NOAGGREGATION", szPrompt);
        break;

    case DSERR_BUFFERLOST:
        sprintf(szErrorString, "%s returned DSERR_BUFFERLOST", szPrompt);
        break;

    case DSERR_OTHERAPPHASPRIO:
        sprintf(szErrorString, "%s returned DSERR_OTHERAPPHASPRIO", szPrompt);
        break;

    case DSERR_UNINITIALIZED:
        sprintf(szErrorString, "%s returned DSERR_UNINITIALIZED", szPrompt);
        break;

    case DSERR_NOINTERFACE:
        sprintf(szErrorString, "%s returned DSERR_NOINTERFACE", szPrompt);
        break;

/*
    case DSERR_HWUNAVAIL:
        sprintf(szErrorString, "%s returned DSERR_HWUNAVAIL", szPrompt);
        break;
*/
    default:
        sprintf(szErrorString, "%s returned unknown error code (0x%08x)", szPrompt, result);
        break;
    }

    Log( nLog, szErrorString );
}

void _stdcall Log_DSCooperativeLevel (int nLog, DWORD dwLevel)
{
    CHAR szTemp[64];

    strcpy(szTemp, "Cooperative Level: ");

    switch (dwLevel) {
    case DSSCL_EXCLUSIVE:
        strcat(szTemp, "DSSCL_EXCLUSIVE ");
        break;

    case DSSCL_NORMAL:
        strcat(szTemp, "DSSCL_NORMAL ");
        break;

    case DSSCL_PRIORITY:
        strcat(szTemp, "DSSCL_PRIORITY  ");
        break;

    case DSSCL_WRITEPRIMARY:
        strcat(szTemp, "DSSCL_WRITEPRIMARY ");
        break;
    }

    Log(nLog, szTemp);
}

void _stdcall Log_GUID (int nLog, LPGUID lpGuid)
{
    OLECHAR     wszGuid[256];

    if (lpGuid == NULL)
    {
        Log(nLog, "  NULL GUID Pointer");
    }
    else
    {
        StringFromGUID2(*lpGuid, wszGuid, 256);
        Log(nLog, "  GUID: %ws", wszGuid);
    }
}

void _stdcall Log_GUID (int nLog, REFGUID Guid)
{
    OLECHAR     wszGuid[256];
    StringFromGUID2(Guid, wszGuid, 256);

    Log(nLog, "  GUID: %ws", wszGuid);
}


void Log_DSCaps (int nLog, LPDSCAPS lpdscaps)
{
    // BUGBUG assuming the flag string will never exceed FLAG_BUFFER_LENGTH
    CHAR   szFlagBuffer[FLAG_BUFFER_LENGTH];
    int     nPos    = 0;
    BOOL    bFirst  = TRUE;

    if (lpdscaps == NULL)
    {
        Log(nLog, "DSCAPS : NULL pointer");
        return;
    }

    Log(nLog, "DSCAPS:");
    Log(nLog,
            "  dwSize                   = %lu", lpdscaps->dwSize);

    nPos += sprintf(szFlagBuffer + nPos,
                    "  dwFlags                           = ");
    if (lpdscaps->dwFlags & DSCAPS_CERTIFIED)
    {
        APPEND_FLAG(" DSCAPS_CERTIFIED");
    }
    if (lpdscaps->dwFlags & DSCAPS_CONTINUOUSRATE)
    {
        APPEND_FLAG(" DSCAPS_CONTINUOUSRATE");
    }
    if (lpdscaps->dwFlags & DSCAPS_EMULDRIVER)
    {
        APPEND_FLAG(" DSCAPS_EMULDRIVER");
    }
    if (lpdscaps->dwFlags & DSCAPS_PRIMARY16BIT)
    {
        APPEND_FLAG(" DSCAPS_PRIMARY16BIT");
    }
    if (lpdscaps->dwFlags & DSCAPS_PRIMARY8BIT)
    {
        APPEND_FLAG(" DSCAPS_PRIMARY8BIT");
    }
    if (lpdscaps->dwFlags & DSCAPS_PRIMARYMONO)
    {
        APPEND_FLAG(" DSCAPS_PRIMARYMONO");
    }
    if (lpdscaps->dwFlags & DSCAPS_PRIMARYSTEREO)
    {
        APPEND_FLAG(" DSCAPS_PRIMARYSTEREO");
    }
    if (lpdscaps->dwFlags & DSCAPS_SECONDARY16BIT)
    {
        APPEND_FLAG(" DSCAPS_SECONDARY16BIT");
    }
    if (lpdscaps->dwFlags & DSCAPS_SECONDARY8BIT)
    {
        APPEND_FLAG(" DSCAPS_SECONDARY8BIT");
    }
    if (lpdscaps->dwFlags & DSCAPS_SECONDARYMONO)
    {
        APPEND_FLAG(" DSCAPS_SECONDARYMONO");
    }
    if (lpdscaps->dwFlags & DSCAPS_SECONDARYSTEREO)
    {
        APPEND_FLAG(" DSCAPS_SECONDARYSTEREO");
    }

    Log(nLog, szFlagBuffer);
    Log(nLog, "  dwMinSecondarySampleRate          = %lu",
            lpdscaps->dwMinSecondarySampleRate);
    Log(nLog, "  dwMaxSecondarySampleRate          = %lu",
            lpdscaps->dwMaxSecondarySampleRate);
    Log(nLog, "  dwPrimaryBuffers                  = %lu",
            lpdscaps->dwPrimaryBuffers);
    Log(nLog, "  dwMaxHwMixingAllBuffers           = %lu",
            lpdscaps->dwMaxHwMixingAllBuffers);
    Log(nLog, "  dwMaxHwMixingStaticBuffers        = %lu",
            lpdscaps->dwMaxHwMixingStaticBuffers);
    Log(nLog, "  dwMaxHwMixingStreamingBuffers     = %lu",
            lpdscaps->dwMaxHwMixingStreamingBuffers);
    Log(nLog, "  dwFreeHwMixingAllBuffers          = %lu",
            lpdscaps->dwFreeHwMixingAllBuffers);
    Log(nLog, "  dwFreeHwMixingStaticBuffers       = %lu",
            lpdscaps->dwFreeHwMixingStaticBuffers);
    Log(nLog, "  dwFreeHwMixingStreamingBuffers    = %lu",
            lpdscaps->dwFreeHwMixingStreamingBuffers);
    Log(nLog, "  dwMaxHw3DAllBuffers               = %lu",
            lpdscaps->dwMaxHw3DAllBuffers);
    Log(nLog, "  dwMaxHw3DStaticBuffers            = %lu",
            lpdscaps->dwMaxHw3DStaticBuffers);
    Log(nLog, "  dwMaxHw3DStreamingBuffers         = %lu",
            lpdscaps->dwMaxHw3DStreamingBuffers);
    Log(nLog, "  dwTotalHwMemBytes                 = %lu",
            lpdscaps->dwTotalHwMemBytes);
    Log(nLog, "  dwFreeHwMemBytes                  = %lu",
            lpdscaps->dwFreeHwMemBytes);
    Log(nLog, "  dwMaxContigFreeHwMemBytes         = %lu",
            lpdscaps->dwMaxContigFreeHwMemBytes);
    Log(nLog, "  dwUnlockTransferRateHwBuffers     = %lu",
            lpdscaps->dwUnlockTransferRateHwBuffers);
    Log(nLog, "  dwPlayCpuOverheadSwBuffers        = %lu",
            lpdscaps->dwPlayCpuOverheadSwBuffers);
    Log(nLog, "  dwReserved1                       = 0x%08x",
            lpdscaps->dwReserved1);
    Log(nLog, "  dwReserved2                       = 0x%08x",
            lpdscaps->dwReserved2);
}

void _stdcall Log_DSBCapsFlags(int nLog, DWORD dwFlags)
{
    // BUGBUG assuming the flag string will never exceed FLAG_BUFFER_LENGTH
    CHAR   szFlagBuffer[FLAG_BUFFER_LENGTH];
    int     nPos    = 0;
    BOOL    bFirst  = TRUE;

    nPos += sprintf(szFlagBuffer + nPos,
                    "  dwFlags               = ");
    if (dwFlags & DSBCAPS_CTRL3D)
    {
        APPEND_FLAG (" DSBCAPS_CTRL3D");
    }
    if (dwFlags & DSBCAPS_CTRLFREQUENCY)
    {
        APPEND_FLAG (" DSBCAPS_CTRLFREQUENCY");
    }
    if (dwFlags & DSBCAPS_CTRLPAN)
    {
        APPEND_FLAG (" DSBCAPS_CTRLPAN");
    }
    if (dwFlags & DSBCAPS_CTRLVOLUME)
    {
        APPEND_FLAG (" DSBCAPS_CTRLVOLUME");
    }
    if (dwFlags & DSBCAPS_GETCURRENTPOSITION2)
    {
        APPEND_FLAG (" DSBCAPS_GETCURRENTPOSITION2");
    }
    if (dwFlags & DSBCAPS_GLOBALFOCUS)
    {
        APPEND_FLAG (" DSBCAPS_GLOBALFOCUS");
    }
    if (dwFlags & DSBCAPS_LOCHARDWARE)
    {
        APPEND_FLAG (" DSBCAPS_LOCHARDWARE");
    }
    if (dwFlags & DSBCAPS_MUTE3DATMAXDISTANCE)
    {
        APPEND_FLAG (" DSBCAPS_MUTE3DATMAXDISTANCE");
    }
    if (dwFlags & DSBCAPS_PRIMARYBUFFER)
    {
        APPEND_FLAG (" DSBCAPS_PRIMARYBUFFER");
    }
    if (dwFlags & DSBCAPS_STATIC)
    {
        APPEND_FLAG (" DSBCAPS_STATIC");
    }
    if (dwFlags & DSBCAPS_STICKYFOCUS)
    {
        APPEND_FLAG (" DSBCAPS_STIKYFOCUS");
    }

    if (dwFlags & DSBCAPS_LOCDEFER)
    {
        APPEND_FLAG (" DSBCAPS_LOCDEFER");
    }

    Log(nLog, szFlagBuffer);
}

void _stdcall Log_DSBCaps (int nLog, LPDSBCAPS lpdsbcaps)
{
    if (lpdsbcaps == NULL)
    {
        Log(nLog, "DSBCAPS : NULL pointer");
        return;
    }

    Log(nLog, "DSBCAPS :");
    Log(nLog,
            "  dwSize                            = %lu", lpdsbcaps->dwSize);
    Log_DSBCapsFlags(nLog, lpdsbcaps->dwFlags);
    Log(nLog, "  dwBufferBytes           = %lu", lpdsbcaps->dwBufferBytes);
    Log(nLog, "  dwUnlockTransferRate    = %lu", lpdsbcaps->dwUnlockTransferRate);
    Log(nLog, "  dwPlayCpuOverhead       = %lu", lpdsbcaps->dwPlayCpuOverhead);
}

void _stdcall Log_DSCCaps (int nLog, LPDSCCAPS lpdsccaps)
{
    // BUGBUG assuming the flag string will never exceed FLAG_BUFFER_LENGTH
    CHAR   szFlagBuffer[FLAG_BUFFER_LENGTH];
    int     nPos    = 0;
    BOOL    bFirst  = TRUE;

    if (lpdsccaps == NULL)
    {
        Log(nLog, "DSCCAPS : NULL pointer");
        return;
    }

    Log(nLog, "DSCCAPS :");
    Log(nLog,
        "  dwSize                            = %lu", lpdsccaps->dwSize);

    nPos += sprintf(szFlagBuffer + nPos,
                    "  dwFlags                           = ");
    if (lpdsccaps->dwFlags & DSCCAPS_EMULDRIVER)
    {
        APPEND_FLAG(" DSCCAPS_EMULDRIVER");
    }

    Log(nLog, szFlagBuffer);
    Log(nLog, "  dwFormats    = 0x%08x", lpdsccaps->dwFormats);
    Log(nLog, "  dwChannels   = 0x%08x", lpdsccaps->dwChannels);
}

void _stdcall Log_DSCBCapsFlags(int nLog, DWORD dwFlags)
{
    // BUGBUG assuming the flag string will never exceed FLAG_BUFFER_LENGTH
    CHAR   szFlagBuffer[FLAG_BUFFER_LENGTH];
    int     nPos    = 0;
    BOOL    bFirst  = TRUE;

    nPos += sprintf(szFlagBuffer + nPos,
                    "  dwFlags                           = ");
    if (dwFlags & DSCBCAPS_WAVEMAPPED)
    {
        APPEND_FLAG (" DSCBCAPS_WAVEMAPPED");
    }

    Log(nLog, szFlagBuffer);
}

void _stdcall Log_DSCBCaps (int nLog, LPDSCBCAPS lpdscbcaps)
{
    if (lpdscbcaps == NULL)
    {
        Log(nLog, "DSCBCAPS : NULL pointer");
        return;
    }

    Log(nLog, "DSCBCAPS :");
    Log(nLog,
            "  dwSize                            = %lu", lpdscbcaps->dwSize);
    Log_DSCBCapsFlags(nLog, lpdscbcaps->dwFlags);
    Log(nLog, "  dwBufferBytes    = %lu", lpdscbcaps->dwBufferBytes);
    Log(nLog, "  dwReserved       = 0x%08x", lpdscbcaps->dwReserved);
}

void _stdcall Log_SpeakerConfig (int nLog, DWORD dwSpkrCfg)
{
    CHAR szConfiguration[255];
    ZeroMemory (szConfiguration, sizeof(szConfiguration));

    Log(nLog, "Speaker configuration:");

    if (DSSPEAKER_CONFIG(dwSpkrCfg) == DSSPEAKER_HEADPHONE)
    {
        sprintf(szConfiguration, "  DSSPEAKER_HEADPHONE");
    }
    else if (DSSPEAKER_CONFIG(dwSpkrCfg) == DSSPEAKER_MONO)
    {
        sprintf(szConfiguration, "  DSSPEAKER_MONO");
    }
    else if (DSSPEAKER_CONFIG(dwSpkrCfg) == DSSPEAKER_QUAD)
    {
        sprintf(szConfiguration, "  DSSPEAKER_QUAD");
    }
    else if (DSSPEAKER_CONFIG(dwSpkrCfg) == DSSPEAKER_SURROUND)
    {
        sprintf(szConfiguration, "  DSSPEAKER_SURROUND");
    }
    else if (DSSPEAKER_CONFIG(dwSpkrCfg) == DSSPEAKER_STEREO)
    {
        if (DSSPEAKER_GEOMETRY(dwSpkrCfg) == DSSPEAKER_GEOMETRY_WIDE)
            sprintf(szConfiguration, "  DSSPEAKER_STEREO | DSSPEAKER_GEOMETRY_WIDE");
        else if (DSSPEAKER_GEOMETRY(dwSpkrCfg) == DSSPEAKER_GEOMETRY_NARROW)
            sprintf(szConfiguration, "  DSSPEAKER_STEREO | DSSPEAKER_GEOMETRY_NARROW");
        else if (DSSPEAKER_GEOMETRY(dwSpkrCfg) == DSSPEAKER_GEOMETRY_MIN)
            sprintf(szConfiguration, "  DSSPEAKER_STEREO | DSSPEAKER_GEOMETRY_MIN");
        else if (DSSPEAKER_GEOMETRY(dwSpkrCfg) == DSSPEAKER_GEOMETRY_MAX)
            sprintf(szConfiguration, "  DSSPEAKER_STEREO | DSSPEAKER_GEOMETRY_MAX");
        else
            sprintf(szConfiguration, "  DSSPEAKER_STEREO");
    }
    else if (DSSPEAKER_CONFIG(dwSpkrCfg) == DSSPEAKER_5POINT1)
    {
        sprintf(szConfiguration, "  DSSPEAKER_5POINT1");
    }
    else
    {
        sprintf(szConfiguration, "  Unknown configuration");
    }

    Log(nLog, szConfiguration);
}

void _stdcall Log_WaveFormatEx(int nLevel, LPCWAVEFORMATEX lpwfx)
{
    if(!lpwfx)
    {
        Log(nLevel, "LPWAVEFORMATEX:  0x%p", lpwfx);
    }
    else
    {
        Log_wFormatTag(nLevel, lpwfx->wFormatTag);
        Log(nLevel, "  nChannels:        %d", lpwfx->nChannels);
        Log(nLevel, "  nSamplesPerSec:   %d", lpwfx->nSamplesPerSec);
        Log(nLevel, "  nAvgBytesPerSec:  %d", lpwfx->nAvgBytesPerSec);
        Log(nLevel, "  nBlockAlign:      %d", lpwfx->nBlockAlign);
        Log(nLevel, "  wBitsPerSample:   %d", lpwfx->wBitsPerSample);
    }
}

void Log_wFormatTag (int nLevel, WORD wFmt)
{
    CHAR    szFormat[FLAG_BUFFER_LENGTH] = " ";

    Log(nLevel, "wFormatTag:       0x%04x", wFmt);

    switch(wFmt)
    {
        case WAVE_FORMAT_PCM:
            sprintf(szFormat, "WAVE_FORMAT_PCM");
            break;

        case WAVE_FORMAT_ADPCM:
            sprintf (szFormat, "WAVE_FORMAT_ADPCM");
            break;

        case WAVE_FORMAT_IBM_CVSD   :
            sprintf (szFormat, "WAVE_FORMAT_IBM_CVSD   ");
            break;

        case WAVE_FORMAT_ALAW       :
            sprintf (szFormat, "WAVE_FORMAT_ALAW       ");
            break;

        case WAVE_FORMAT_MULAW      :
            sprintf (szFormat, "WAVE_FORMAT_MULAW      ");
            break;

        case WAVE_FORMAT_OKI_ADPCM  :
            sprintf (szFormat, "WAVE_FORMAT_OKI_ADPCM  ");
            break;

        case WAVE_FORMAT_IMA_ADPCM  :
            sprintf (szFormat, "WAVE_FORMAT_IMA_ADPCM  ");
            break;

        case WAVE_FORMAT_MEDIASPACE_ADPCM:
            sprintf (szFormat, "WAVE_FORMAT_MEDIASPACE_ADPCM");
            break;

        case WAVE_FORMAT_SIERRA_ADPCM:
            sprintf (szFormat, "WAVE_FORMAT_SIERRA_ADPCM");
            break;

        case WAVE_FORMAT_G723_ADPCM :
            sprintf (szFormat, "WAVE_FORMAT_G723_ADPCM ");
            break;

        case WAVE_FORMAT_DIGISTD    :
            sprintf (szFormat, "WAVE_FORMAT_DIGISTD    ");
            break;

        case WAVE_FORMAT_DIGIFIX    :
            sprintf (szFormat, "WAVE_FORMAT_DIGIFIX    ");
            break;

        case WAVE_FORMAT_DIALOGIC_OKI_ADPCM :
            sprintf (szFormat, "WAVE_FORMAT_DIALOGIC_OKI_ADPCM ");
            break;

        case WAVE_FORMAT_YAMAHA_ADPCM       :
            sprintf (szFormat, "WAVE_FORMAT_YAMAHA_ADPCM       ");
            break;

        case WAVE_FORMAT_SONARC :
            sprintf (szFormat, "WAVE_FORMAT_SONARC ");
            break;

        case WAVE_FORMAT_DSPGROUP_TRUESPEECH  :
            sprintf (szFormat, "WAVE_FORMAT_DSPGROUP_TRUESPEECH  ");
            break;

        case WAVE_FORMAT_ECHOSC1 :
            sprintf (szFormat, "WAVE_FORMAT_ECHOSC1 ");
            break;

        case WAVE_FORMAT_AUDIOFILE_AF36:
            sprintf (szFormat, "WAVE_FORMAT_AUDIOFILE_AF36");
            break;

        case WAVE_FORMAT_APTX :
            sprintf (szFormat, "WAVE_FORMAT_APTX ");
            break;

        case WAVE_FORMAT_AUDIOFILE_AF10    :
            sprintf (szFormat, "WAVE_FORMAT_AUDIOFILE_AF10    ");
            break;

        case WAVE_FORMAT_DOLBY_AC2 :
            sprintf (szFormat, "WAVE_FORMAT_DOLBY_AC2 ");
            break;

        case WAVE_FORMAT_GSM610    :
            sprintf (szFormat, "WAVE_FORMAT_GSM610    ");
            break;

        case WAVE_FORMAT_ANTEX_ADPCME      :
            sprintf (szFormat, "WAVE_FORMAT_ANTEX_ADPCME      ");
            break;

        case WAVE_FORMAT_CONTROL_RES_VQLPC :
            sprintf (szFormat, "WAVE_FORMAT_CONTROL_RES_VQLPC ");
            break;

        case WAVE_FORMAT_DIGIREAL   :
            sprintf (szFormat, "WAVE_FORMAT_DIGIREAL   ");
            break;

        case WAVE_FORMAT_DIGIADPCM  :
            sprintf (szFormat, "WAVE_FORMAT_DIGIADPCM  ");
            break;

        case WAVE_FORMAT_CONTROL_RES_CR10  :
            sprintf (szFormat, "WAVE_FORMAT_CONTROL_RES_CR10  ");
            break;

        case WAVE_FORMAT_NMS_VBXADPCM  :
            sprintf (szFormat, "WAVE_FORMAT_NMS_VBXADPCM  ");
            break;

        case WAVE_FORMAT_CS_IMAADPCM :
            sprintf (szFormat, "WAVE_FORMAT_CS_IMAADPCM ");
            break;

        case WAVE_FORMAT_G721_ADPCM :
            sprintf (szFormat, "WAVE_FORMAT_G721_ADPCM ");
            break;

        case WAVE_FORMAT_MPEG       :
            sprintf (szFormat, "WAVE_FORMAT_MPEG       ");
            break;

        case WAVE_FORMAT_CREATIVE_ADPCM :
            sprintf (szFormat, "WAVE_FORMAT_CREATIVE_ADPCM ");
            break;

        case WAVE_FORMAT_CREATIVE_FASTSPEECH8 :
            sprintf (szFormat, "WAVE_FORMAT_CREATIVE_FASTSPEECH8 ");
            break;

        case WAVE_FORMAT_CREATIVE_FASTSPEECH10:
            sprintf (szFormat, "WAVE_FORMAT_CREATIVE_FASTSPEECH10");
            break;

        case WAVE_FORMAT_FM_TOWNS_SND :
            sprintf (szFormat, "WAVE_FORMAT_FM_TOWNS_SND ");
            break;

        case WAVE_FORMAT_OLIGSM   :
            sprintf (szFormat, "WAVE_FORMAT_OLIGSM   ");
            break;

        case WAVE_FORMAT_OLIADPCM :
            sprintf (szFormat, "WAVE_FORMAT_OLIADPCM ");
            break;

        case WAVE_FORMAT_OLICELP  :
            sprintf (szFormat, "WAVE_FORMAT_OLICELP");
            break;

        case WAVE_FORMAT_OLISBC   :
            sprintf (szFormat, "WAVE_FORMAT_OLISBC");
            break;

        case WAVE_FORMAT_OLIOPR   :
            sprintf (szFormat, "WAVE_FORMAT_OLIOPR");
            break;

        case WAVE_FORMAT_DEVELOPMENT:
            sprintf (szFormat, "WAVE_FORMAT_DEVELOPMENT");
            break;

        default :
            sprintf (szFormat, "Unknown Wave Format");
     }

     Log(nLevel, "  wFormatTag:       %s", szFormat);
}

void _stdcall Log_DSBufferDesc(int nLevel, LPCDSBUFFERDESC lpdsbd)
{
    if (NULL == lpdsbd)
        return;

//    if (!IsBadReadPtr(lpdsbd, sizeof(DSBUFFERDESC)))
    if (!IsBadReadPtr(lpdsbd, lpdsbd->dwSize))
    {
        Log(nLevel, "  dwSize: %lu", lpdsbd->dwSize);
        Log_DSBCapsFlags(nLevel, lpdsbd->dwFlags);
        Log(nLevel, "  dwBufferBytes:  %lu", lpdsbd->dwBufferBytes);
        Log(nLevel, "  dwReserved:  %lu", lpdsbd->dwReserved);
        Log_WaveFormatEx(nLevel, lpdsbd->lpwfxFormat);
        if (sizeof(DSBUFFERDESC) <= lpdsbd->dwSize)
        {
            Log_DS3DAlgorithm(nLevel, (LPGUID) &(lpdsbd->guid3DAlgorithm));
        }
    }
}

void _stdcall Log_DSCBufferDesc (int nLevel, LPCDSCBUFFERDESC lpdscbd)
{
    if (!IsBadReadPtr(lpdscbd, sizeof(DSCBUFFERDESC)))
    {
        Log(nLevel, "  dwSize:           %lu", lpdscbd->dwSize);
        Log_DSCBCapsFlags(nLevel, lpdscbd->dwFlags);
        Log(nLevel, "  dwBufferBytes:    %lu", lpdscbd->dwBufferBytes);
        Log(nLevel, "  dwReserved:       %lu", lpdscbd->dwReserved);
        Log(nLevel, "  LPWAVEFORMATEX: 0x%p", lpdscbd->lpwfxFormat);
        Log_WaveFormatEx(nLevel, lpdscbd->lpwfxFormat);
    }
}

void _stdcall Log_DSBLockFlags(int nLog, DWORD dwFlags)
{
    CHAR szFlags[255];

    ZeroMemory (szFlags, sizeof(szFlags));

    Log(nLog, "Lock flags:");

    if (dwFlags & DSBLOCK_FROMWRITECURSOR)
        sprintf(szFlags, "  DSBLOCK_FROMWRITECURSOR");
    else if (dwFlags & DSBLOCK_ENTIREBUFFER)
        sprintf(szFlags, "  DSBLOCK_ENTIREBUFFER");

    Log(nLog, szFlags);
}

void _stdcall Log_DSCBLockFlags(int nLog, DWORD dwFlags)
{
    CHAR szFlags[255];

    Log(nLog, "Lock flags:");
    ZeroMemory (szFlags, sizeof(szFlags));

    if (dwFlags & DSCBLOCK_ENTIREBUFFER)
        sprintf(szFlags, "  DSCBLOCK_ENTIREBUFFER");

    Log(nLog, szFlags);
}

void _stdcall Log_DSBPlayFlags(int nLog, DWORD dwFlags)
{
    CHAR   szFlagBuffer[FLAG_BUFFER_LENGTH];
    int     nPos    = 0;
    BOOL    bFirst  = TRUE;

    ZeroMemory (szFlagBuffer, sizeof (szFlagBuffer));

    nPos += sprintf(szFlagBuffer + nPos,
                    "  dwFlags  = ");
    if (dwFlags & DSBPLAY_LOOPING)
    {
        APPEND_FLAG (" DSBPLAY_LOOPING");
    }

    if (dwFlags & DSBPLAY_LOCHARDWARE)
    {
        APPEND_FLAG (" DSBPLAY_LOCHARDWARE");
    }

    if (dwFlags & DSBPLAY_LOCSOFTWARE)
    {
        APPEND_FLAG (" DSBPLAY_LOCSOFTWARE");
    }

    if (dwFlags & DSBPLAY_TERMINATEBY_TIME)
    {
        APPEND_FLAG (" DSBPLAY_TERMINATEBY_TIME");
    }

    if (dwFlags & DSBPLAY_TERMINATEBY_DISTANCE)
    {
        APPEND_FLAG (" DSBPLAY_TERMINATEBY_DISTANCE");
    }

    if (dwFlags & DSBPLAY_TERMINATEBY_PRIORITY)
    {
        APPEND_FLAG (" DSBPLAY_TERMINATEBY_PRIORITY");
    }

    Log(nLog, szFlagBuffer);
}

void _stdcall Log_DSCBStartFlags (int nLog, DWORD dwFlags)
{
    CHAR szFlags[255];
    ZeroMemory (szFlags, sizeof(szFlags));

    Log(nLog, "Start flags:");

    if (dwFlags & DSCBSTART_LOOPING)
        sprintf(szFlags, "  DSCBSTART_LOOPING");

    Log(nLog, szFlags);
}

void _stdcall Log_DSBStatus (int nLog, DWORD dwFlags)
{
    CHAR   szFlagBuffer[FLAG_BUFFER_LENGTH];
    int     nPos    = 0;
    BOOL    bFirst  = TRUE;

    nPos += sprintf(szFlagBuffer + nPos,
                    "  dwStatus  = ");
    if (dwFlags & DSBSTATUS_PLAYING)
    {
        APPEND_FLAG (" DSBSTATUS_PLAYING");
    }

    if (dwFlags & DSBSTATUS_LOOPING)
    {
        APPEND_FLAG (" DSBSTATUS_LOOPING");
    }

    if (dwFlags & DSBSTATUS_BUFFERLOST)
    {
        APPEND_FLAG (" DSBSTATUS_BUFFERLOST");
    }

    if (dwFlags & DSBSTATUS_LOCSOFTWARE)
    {
        APPEND_FLAG (" DSBSTATUS_LOCSOFTWARE");
    }

    if (dwFlags & DSBSTATUS_LOCHARDWARE)
    {
        APPEND_FLAG (" DSBSTATUS_LOCHARDWARE");
    }
/*
    if (dwFlags & DSBSTATUS_LOCDEFERED)
    {
        APPEND_FLAG (" DSBSTATUS_LOCDEFERED");
    }
*/
    if (dwFlags & DSBSTATUS_TERMINATED)
    {
        APPEND_FLAG (" DSBSTATUS_TERMINATED");
    }



    Log(nLog, szFlagBuffer);
}

void _stdcall Log_DSCBStatus (int nLog, DWORD dwFlags)
{
    CHAR   szFlagBuffer[FLAG_BUFFER_LENGTH];
    int     nPos    = 0;
    BOOL    bFirst  = TRUE;

    nPos += sprintf(szFlagBuffer + nPos,
                    "  dwStatus  = ");
    if (dwFlags & DSCBSTATUS_CAPTURING)
    {
        APPEND_FLAG (" DSCBSTATUS_CAPTURING");
    }
    if (dwFlags & DSCBSTATUS_LOOPING)
    {
        APPEND_FLAG (" DSCBSTATUS_LOOPING");
    }

    Log(nLog, szFlagBuffer);
}

void _stdcall Log_DS3DApplyMode (int nLog, DWORD dwMode)
{
    CHAR szMode[255];
    ZeroMemory (szMode, sizeof(szMode));

    if (dwMode == DS3D_DEFERRED)
        sprintf(szMode, "  DS3D_DEFERRED");
    else if (dwMode == DS3D_IMMEDIATE)
        sprintf(szMode, "  DS3D_IMMEDIATE");

    Log(nLog, szMode);
}

void _stdcall Log_DS3DBMode (int nLog, DWORD dwMode)
{
    CHAR szMode[255];
    ZeroMemory (szMode, sizeof(szMode));

    switch (dwMode)
    {
    case DS3DMODE_DISABLE:
        sprintf(szMode, "  DS3DMODE_DISABLE");
        break;

    case DS3DMODE_HEADRELATIVE:
        sprintf(szMode, "  DS3DMODE_HEADRELATIVE");
        break;

    case DS3DMODE_NORMAL:
        sprintf(szMode, "  DS3DMODE_NORMAL");
        break;

    default:
        sprintf(szMode, "  Unknown mode");
        break;
    }

    Log(nLog, szMode);
}

void Log_D3DVector(int nLog, const struct _D3DVECTOR * lpv)
{
    if (lpv == NULL)
        return;

    Log_tstLof(nLog, "    %19.9g", lpv->x);
    Log_tstLof(nLog, "    %19.9g", lpv->y);
    Log_tstLof(nLog, "    %19.9g", lpv->z);
}

void Log_DS3DLAllParameters(int nLog, LPCDS3DLISTENER lpl)
{
    if (lpl == NULL)
        return;

    Log(nLog, "  dwSize               = %lu", lpl->dwSize);
    Log(nLog, "  vPosition:");
    Log_D3DVector(nLog, &(lpl->vPosition));
    Log(nLog, "  vVelocity:");
    Log_D3DVector(nLog, &(lpl->vVelocity));
    Log(nLog, "  vOrientFront:");
    Log_D3DVector(nLog, &(lpl->vOrientFront));
    Log(nLog, "  vOrientTop:");
    Log_D3DVector(nLog, &(lpl->vOrientTop));

    Log_tstLof(nLog,  "  flDistanceFactor     = %19.9g", lpl->flDistanceFactor);
    Log_tstLof(nLog,  "  flRolloffFactor      = %19.9g", lpl->flRolloffFactor);
    Log_tstLof(nLog,  "  flDopplerFactor      = %19.9g", lpl->flDopplerFactor);
}

void Log_DS3DBAllParameters(int nLog, LPCDS3DBUFFER lpb)
{
    if (lpb == NULL)
        return;

    Log(nLog, "  dwSize               = %lu", lpb->dwSize);
    Log(nLog, "  vPosition:");
    Log_D3DVector(nLog, &(lpb->vPosition));
    Log(nLog, "  vVelocity:");
    Log_D3DVector(nLog, &(lpb->vVelocity));
    Log(nLog, "  dwInsideConeAngle    = %lu", lpb->dwInsideConeAngle);
    Log(nLog, "  dwOutsideConeAngle   = %lu", lpb->dwOutsideConeAngle);
    Log(nLog, "  vConeOrientation:");
    Log_D3DVector(nLog, &(lpb->vConeOrientation));
    Log(nLog, "  lConeOutsideVolume   = %ld", lpb->lConeOutsideVolume);

    Log_tstLof(nLog,  "  flMinDistance        = %19.9g", lpb->flMinDistance);
    Log_tstLof(nLog,  "  flMaxDistance        = %19.9g", lpb->flMaxDistance);

    Log(nLog, "  dwMode :");
    Log_DS3DBMode(nLog, lpb->dwMode);
}

void Log_DSNPositionNotifies (int nLog, DWORD dwNum, LPCDSBPOSITIONNOTIFY notifies)
{
    ULONG i;
    if (notifies == NULL)
        return;

    Log(nLog, "  %lu notification positions:", dwNum);

    for(i=0;i<(ULONG) dwNum;i++)
    {
        Log(nLog, "  Notification %lu:", i);
        Log(nLog, "      dwOffset      =       0x%08x", notifies[i].dwOffset);
        Log(nLog, "      hEventNotify  =       0x%p", notifies[i].hEventNotify);
    }
}

void Log_tstLof(int nLevel, CHAR * szFormat, double flNumber)
{
    //BUGBUG, assuming the printout never exceeds 255 characters
    CHAR szBuffer[256];
    assert(szFormat);
    _stprintf(szBuffer, szFormat, flNumber);

    Log(nLevel, szBuffer);
}


//===========================================================================
// dmthLogWAVEFORMATEX
//
// Logs the contents of WAVEFORMATEX structure
//
// Parameters:
//  UINT                    uLogLevel   - fnshell logging level
//  WAVEFORMATEX *          lpwfx - pointer to DMUS_SCRIPT_ERRORINFO
//
// Returns: nothing
//
// History:
//  02/23/2000 - danhaff - created
//===========================================================================
void _stdcall dmthLogWAVEFORMATEX(UINT uLogLevel, LPWAVEFORMATEX lpwfx)
{
    //////////////////////////////////////////////////////////////////
    // validate lpwfx
    //////////////////////////////////////////////////////////////////
    if(!helpIsValidPtr((void*)lpwfx, sizeof(WAVEFORMATEX), FALSE))
    {
        fnsLog(uLogLevel, "Invalid LPWAVEFORMATEX pointer (%08Xh)..."
                "Unable to log structure contents", lpwfx);
        return;
    }

    if(!lpwfx)
    {
        fnsLog(uLogLevel, "LPWAVEFORMATEX:  0x%p", lpwfx);
    }
    else
    {
        dmthLogFormatTag(uLogLevel, lpwfx->wFormatTag);

        //Should we log the original WFX in this case?  I am not, for now.
        if (WAVE_FORMAT_DRM == lpwfx->wFormatTag)
        {
            DRMWAVEFORMAT * p = (DRMWAVEFORMAT *)lpwfx;
            fnsLog(uLogLevel, "WFX                           ");
            fnsLog(uLogLevel, "  nChannels:                  %d", p->wfx.nChannels);
            fnsLog(uLogLevel, "  nSamplesPerSec:             %d", p->wfx.nSamplesPerSec);
            fnsLog(uLogLevel, "  nAvgBytesPerSec:            %d", p->wfx.nAvgBytesPerSec);
            fnsLog(uLogLevel, "  nBlockAlign:                %d", p->wfx.nBlockAlign);
            fnsLog(uLogLevel, "  wBitsPerSample:             %d", p->wfx.wBitsPerSample);
            fnsLog(uLogLevel, "  cbSize:                     %d", p->wfx.cbSize);
            fnsLog(uLogLevel, "");
            fnsLog(uLogLevel, "  wReserved:                  %d", p->wReserved);
            fnsLog(uLogLevel, "  ulContentId:                %d", p->ulContentId);
            fnsLog(uLogLevel, "WFXSECURE                     ");
            dmthLogWAVEFORMATEX(uLogLevel, &p->wfxSecure);
        }
        else if (WAVE_FORMAT_EXTENSIBLE == lpwfx->wFormatTag)
        {
            WAVEFORMATEXTENSIBLE *  p = (WAVEFORMATEXTENSIBLE *)lpwfx;
            fnsLog(uLogLevel, "  nChannels:                  %d", p->Format.nChannels);
            fnsLog(uLogLevel, "  nSamplesPerSec:             %d", p->Format.nSamplesPerSec);
            fnsLog(uLogLevel, "  nAvgBytesPerSec:            %d", p->Format.nAvgBytesPerSec);
            fnsLog(uLogLevel, "  nBlockAlign:                %d", p->Format.nBlockAlign);
            fnsLog(uLogLevel, "  wBitsPerSample:             %d", p->Format.wBitsPerSample);
            fnsLog(uLogLevel, "  cbSize:                     %d", p->Format.cbSize);

            fnsLog(uLogLevel, "  wValidBitsPerSample:        %d", p->Samples.wValidBitsPerSample);
            fnsLog(uLogLevel, "  dwChannelMask:              0x%08x", p->dwChannelMask);
            if (IsEqualGUID(p->SubFormat, KSDATAFORMAT_SUBTYPE_PCM))
            {
                fnsLog(uLogLevel, "  SubFormat: KSDATAFORMAT_SUBTYPE_PCM");
            }
            else if (IsEqualGUID(p->SubFormat, KSDATAFORMAT_SUBTYPE_ADPCM))
            {
                fnsLog(uLogLevel, "  SubFormat:                  KSDATAFORMAT_SUBTYPE_ADPCM");
            }
            else if (IsEqualGUID(p->SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
            {
                fnsLog(uLogLevel, "  SubFormat:                  KSDATAFORMAT_SUBTYPE_IEEE_FLOAT");
            }
            else
            {
                fnsLog(uLogLevel, "  SubFormat:                  UNKNOWN");
                dmthLogRawGUID(uLogLevel, p->SubFormat);
            }
        }
        else //go with default PCM-style data.
        {
            fnsLog(uLogLevel, "  nChannels:        %d", lpwfx->nChannels);
            fnsLog(uLogLevel, "  nSamplesPerSec:   %d", lpwfx->nSamplesPerSec);
            fnsLog(uLogLevel, "  nAvgBytesPerSec:  %d", lpwfx->nAvgBytesPerSec);
            fnsLog(uLogLevel, "  nBlockAlign:      %d", lpwfx->nBlockAlign);
            fnsLog(uLogLevel, "  wBitsPerSample:   %d", lpwfx->wBitsPerSample);

            //If we're WAVE_FORMAT_PCM, then we don't touch cbSize.
            if (WAVE_FORMAT_PCM != lpwfx->wFormatTag)
                fnsLog(uLogLevel, "  cbSize        :   %d", lpwfx->cbSize);
        }

    }
}


//===========================================================================
// dmthLogWAVEFORMATEX
//
// Logs a format tag structure
//
// Parameters:
//  UINT                    uLogLevel   - fnshell logging level
//  WORD                    wFmt        - type of waveformat.
//
// Returns: nothing
//
// History:
//  02/23/2000 - danhaff - created
//===========================================================================
void _stdcall dmthLogFormatTag(UINT uLogLevel, WORD wFmt)
{
    CHAR    szFormat[512] = " ";

    ZeroMemory (szFormat, sizeof(szFormat));
    fnsLog(uLogLevel, "wFormatTag:       0x%04x", wFmt);
    switch(wFmt)
    {
        case WAVE_FORMAT_DRM:
            sprintf(szFormat, "WAVE_FORMAT_DRM");
            break;

        case WAVE_FORMAT_EXTENSIBLE:
            sprintf(szFormat, "WAVE_FORMAT_EXTENSIBLE");
            break;

        case WAVE_FORMAT_PCM:
            sprintf(szFormat, "WAVE_FORMAT_PCM");
            break;

        case WAVE_FORMAT_ADPCM:
            sprintf (szFormat, "WAVE_FORMAT_ADPCM");
            break;

        case WAVE_FORMAT_IBM_CVSD   :
            sprintf (szFormat, "WAVE_FORMAT_IBM_CVSD   ");
            break;

        case WAVE_FORMAT_ALAW       :
            sprintf (szFormat, "WAVE_FORMAT_ALAW       ");
            break;

        case WAVE_FORMAT_MULAW      :
            sprintf (szFormat, "WAVE_FORMAT_MULAW      ");
            break;

        case WAVE_FORMAT_OKI_ADPCM  :
            sprintf (szFormat, "WAVE_FORMAT_OKI_ADPCM  ");
            break;

        case WAVE_FORMAT_IMA_ADPCM  :
            sprintf (szFormat, "WAVE_FORMAT_IMA_ADPCM  ");
            break;

        case WAVE_FORMAT_MEDIASPACE_ADPCM:
            sprintf (szFormat, "WAVE_FORMAT_MEDIASPACE_ADPCM");
            break;

        case WAVE_FORMAT_SIERRA_ADPCM:
            sprintf (szFormat, "WAVE_FORMAT_SIERRA_ADPCM");
            break;

        case WAVE_FORMAT_G723_ADPCM :
            sprintf (szFormat, "WAVE_FORMAT_G723_ADPCM ");
            break;

        case WAVE_FORMAT_DIGISTD    :
            sprintf (szFormat, "WAVE_FORMAT_DIGISTD    ");
            break;

        case WAVE_FORMAT_DIGIFIX    :
            sprintf (szFormat, "WAVE_FORMAT_DIGIFIX    ");
            break;

        case WAVE_FORMAT_DIALOGIC_OKI_ADPCM :
            sprintf (szFormat, "WAVE_FORMAT_DIALOGIC_OKI_ADPCM ");
            break;

        case WAVE_FORMAT_YAMAHA_ADPCM       :
            sprintf (szFormat, "WAVE_FORMAT_YAMAHA_ADPCM       ");
            break;

        case WAVE_FORMAT_SONARC :
            sprintf (szFormat, "WAVE_FORMAT_SONARC ");
            break;

        case WAVE_FORMAT_DSPGROUP_TRUESPEECH  :
            sprintf (szFormat, "WAVE_FORMAT_DSPGROUP_TRUESPEECH  ");
            break;

        case WAVE_FORMAT_ECHOSC1 :
            sprintf (szFormat, "WAVE_FORMAT_ECHOSC1 ");
            break;

        case WAVE_FORMAT_AUDIOFILE_AF36:
            sprintf (szFormat, "WAVE_FORMAT_AUDIOFILE_AF36");
            break;

        case WAVE_FORMAT_APTX :
            sprintf (szFormat, "WAVE_FORMAT_APTX ");
            break;

        case WAVE_FORMAT_AUDIOFILE_AF10    :
            sprintf (szFormat, "WAVE_FORMAT_AUDIOFILE_AF10    ");
            break;

        case WAVE_FORMAT_DOLBY_AC2 :
            sprintf (szFormat, "WAVE_FORMAT_DOLBY_AC2 ");
            break;

        case WAVE_FORMAT_GSM610    :
            sprintf (szFormat, "WAVE_FORMAT_GSM610    ");
            break;

        case WAVE_FORMAT_ANTEX_ADPCME      :
            sprintf (szFormat, "WAVE_FORMAT_ANTEX_ADPCME      ");
            break;

        case WAVE_FORMAT_CONTROL_RES_VQLPC :
            sprintf (szFormat, "WAVE_FORMAT_CONTROL_RES_VQLPC ");
            break;

        case WAVE_FORMAT_DIGIREAL   :
            sprintf (szFormat, "WAVE_FORMAT_DIGIREAL   ");
            break;

        case WAVE_FORMAT_DIGIADPCM  :
            sprintf (szFormat, "WAVE_FORMAT_DIGIADPCM  ");
            break;

        case WAVE_FORMAT_CONTROL_RES_CR10  :
            sprintf (szFormat, "WAVE_FORMAT_CONTROL_RES_CR10  ");
            break;

        case WAVE_FORMAT_NMS_VBXADPCM  :
            sprintf (szFormat, "WAVE_FORMAT_NMS_VBXADPCM  ");
            break;

        case WAVE_FORMAT_CS_IMAADPCM :
            sprintf (szFormat, "WAVE_FORMAT_CS_IMAADPCM ");
            break;

        case WAVE_FORMAT_G721_ADPCM :
            sprintf (szFormat, "WAVE_FORMAT_G721_ADPCM ");
            break;

        case WAVE_FORMAT_MPEG       :
            sprintf (szFormat, "WAVE_FORMAT_MPEG       ");
            break;

        case WAVE_FORMAT_CREATIVE_ADPCM :
            sprintf (szFormat, "WAVE_FORMAT_CREATIVE_ADPCM ");
            break;

        case WAVE_FORMAT_CREATIVE_FASTSPEECH8 :
            sprintf (szFormat, "WAVE_FORMAT_CREATIVE_FASTSPEECH8 ");
            break;

        case WAVE_FORMAT_CREATIVE_FASTSPEECH10:
            sprintf (szFormat, "WAVE_FORMAT_CREATIVE_FASTSPEECH10");
            break;

        case WAVE_FORMAT_FM_TOWNS_SND :
            sprintf (szFormat, "WAVE_FORMAT_FM_TOWNS_SND ");
            break;

        case WAVE_FORMAT_OLIGSM   :
            sprintf (szFormat, "WAVE_FORMAT_OLIGSM   ");
            break;

        case WAVE_FORMAT_OLIADPCM :
            sprintf (szFormat, "WAVE_FORMAT_OLIADPCM ");
            break;

        case WAVE_FORMAT_OLICELP  :
            sprintf (szFormat, "WAVE_FORMAT_OLICELP");
            break;

        case WAVE_FORMAT_OLISBC   :
            sprintf (szFormat, "WAVE_FORMAT_OLISBC");
            break;

        case WAVE_FORMAT_OLIOPR   :
            sprintf (szFormat, "WAVE_FORMAT_OLIOPR");
            break;

        case WAVE_FORMAT_DEVELOPMENT:
            sprintf (szFormat, "WAVE_FORMAT_DEVELOPMENT");
            break;

        default :
            sprintf (szFormat, "Unknown Wave Format");
     }

     fnsLog(uLogLevel, "  wFormatTag:       %s", szFormat);
}




LRESULT _cdecl DummyLog (DWORD, LPSTR, ...)
{
    return 0;
}


/**********************************************************************
**********************************************************************/
DWORD _stdcall GetRandomDWORD(DWORD dwModulus)
{
LARGE_INTEGER lint;
BOOL bRet;

    //We should never run this on an OS without QPC available.
    bRet = QueryPerformanceCounter(&lint);
    ASSERT(bRet);


    srand(lint.u.LowPart);
    if (dwModulus)
        return rand() % dwModulus;

    //If our modulus is zero, it's probably because someone got an array of size zero and they're
    //  attempting to pick a random element in it.
    else
    {
        ASSERT(FALSE);
        return 0;
    }


}



/**********************************************************************
**********************************************************************/
D3DVALUE _stdcall GetRandomD3DVALUE
(
 D3DVALUE    flLower,
 D3DVALUE    flUpper,
 D3DVALUE    flResolution
)
{
    DWORD    dwTemp;
    DWORD    dwModulus;

    dwModulus = (DWORD)((flUpper-flLower) / flResolution);
    if (dwModulus == 0)
        dwModulus = 0xffffffff;
    dwTemp = GetRandomDWORD(dwModulus);

    return D3DVAL(dwTemp)*flResolution + flLower;
}


