/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       util.h
 *  Content:    DirectSound utility functions.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  06/07/01    dereks  Created.
 *
 ****************************************************************************/

#ifndef __UTIL_H__
#define __UTIL_H__

//
// Data structures
//

BEGIN_DEFINE_UNION()
    WAVEFORMATEX        wfx;
    XBOXADPCMWAVEFORMAT wfAdpcm;
END_DEFINE_UNION(UNIWAVEFORMAT);

//
// Return code validation
//

__inline HRESULT ValidateResult(HRESULT hr)
{
    if(FAILED(hr))
    {
        __asm int 3;
    }

    return hr;
}

__inline HRESULT ValidatePtr(LPVOID pv)
{
    if(!pv)
    {
        __asm int 3;
    }

    return pv ? DS_OK : DSERR_OUTOFMEMORY;
}

//
// Memory management
//

LPVOID __MALLOC(DWORD dwSize);
VOID __FREE(LPVOID *ppv);

#define MALLOC(p, type, count) \
    ValidatePtr((p) = (type *)__MALLOC(sizeof(type) * (count)))

#define FREE(p) \
    __FREE((LPVOID *)&(p))

#ifdef __cplusplus

//
// XMO helpers
//

HRESULT
ReadXMO
(
    XMediaObject *          pMediaObject,
    LPVOID                  pvBuffer,
    DWORD                   dwBufferSize,
    LPDWORD                 pdwRead
);

HRESULT
WriteXMO
(
    XMediaObject *          pMediaObject,
    LPVOID                  pvBuffer,
    DWORD                   dwBufferSize,
    LPDWORD                 pdwWritten
);

HRESULT
TransformXMO
(
    XMediaObject *          pMediaObject,
    LPCVOID                 pvSource,
    DWORD                   dwSourceSize,
    LPDWORD                 pdwRead,
    LPVOID                  pvDest,
    DWORD                   dwDestSize,
    LPDWORD                 pdwWritten
);

//
// Wave files
//

HRESULT
OpenWaveFile
(
    LPCSTR                  pszFileName,
    LPCWAVEFORMATEX *       ppwfxFormat,
    XFileMediaObject **     ppMediaObject
);

HRESULT
LoadWaveFile
(
    LPCSTR                  pszFileName,
    LPCWAVEFORMATEX *       ppwfxFormat,
    LPVOID *                ppvBuffer,
    LPDWORD                 pdwBufferSize,
    XFileMediaObject **     ppMediaObject
);

void
GetStdWaveFile
(
    LPCWAVEFORMATEX         pwfxFormat,
    LPSTR                   pszFileName
);

LPCSTR
GetStdWaveFile
(
    LPCWAVEFORMATEX         pwfxFormat
);

HRESULT
OpenStdWaveFile
(
    LPCWAVEFORMATEX         pwfxFormat,
    XFileMediaObject **     ppMediaObject
);

HRESULT
LoadStdWaveFile
(
    LPCWAVEFORMATEX         pwfxFormat,
    LPVOID *                ppvBuffer,
    LPDWORD                 pdwBufferSize,
    XFileMediaObject **     ppMediaObject
);

//
// Buffer creation
//

HRESULT
CreateBufferFromFile
(
    LPCSTR                  pszFile, 
    DWORD                   dwFlags, 
    LPCDSMIXBINS            pMixBins, 
    LPDIRECTSOUNDBUFFER     pOutputBuffer, 
    LPDIRECTSOUNDBUFFER *   ppBuffer, 
    LPDWORD                 pdwBufferBytes  = NULL
);

HRESULT
CreateStdBuffer
(
    DWORD                   dwFlags, 
    LPCWAVEFORMATEX         pwfxFormat, 
    LPCDSMIXBINS            pMixBins, 
    LPDIRECTSOUNDBUFFER     pOutputBuffer, 
    LPDIRECTSOUNDBUFFER *   ppBuffer        = NULL, 
    LPDWORD                 pdwBufferBytes  = NULL
);

HRESULT
CreateToneBuffer
(
    DWORD                   dwFlags,
    DWORD                   dwSamplesPerSec,
    DWORD                   dwBitsPerSample,
    DWORD                   dwChannelCount,
    DWORD                   dwToneFrequency,
    DWORD                   dwBufferBytes,
    LPCDSMIXBINS            pMixBins, 
    LPDIRECTSOUNDBUFFER *   ppBuffer
);

HRESULT
CreateImpulseBuffer
(
    DWORD                   dwFlags,
    DWORD                   dwSamplesPerSec,
    DWORD                   dwBitsPerSample,
    DWORD                   dwChannelCount,
    DWORD                   dwPeriodDivisor,
    DWORD                   dwLeadIn,
    DWORD                   dwRollOff,
    LPCDSMIXBINS            pMixBins, 
    LPDIRECTSOUNDBUFFER *   ppBuffer
);

//
// Buffer utilities
//

HRESULT
WaitBuffer
(
    LPDIRECTSOUNDBUFFER     pBuffer,
    BOOL                    fPlaying
);

HRESULT
PlayBuffer
(
    LPDIRECTSOUNDBUFFER     pBuffer,
    DWORD                   dwFlags     = 0,
    DWORD                   dwDelay     = 0,
    BOOL                    fWait       = FALSE
);

HRESULT
PlayBufferToCompletion
(
    LPDIRECTSOUNDBUFFER     pBuffer,
    DWORD                   dwFlags     = 0,
    DWORD                   dwDelay     = 0
);

HRESULT
StopBuffer
(
    LPDIRECTSOUNDBUFFER     pBuffer,
    DWORD                   dwFlags     = 0,
    DWORD                   dwDelay     = 0,
    BOOL                    fReset      = FALSE,
    BOOL                    fWait       = FALSE
);

HRESULT
GetFreeSGEs
(
    LPDWORD                 pdwFreeSGEs
);

HRESULT
ClampBufferSize
(
    LPDWORD                 pdwBufferBytes
);

//
// Stream creation
//

HRESULT
CreateStreamFromFile
(
    LPCSTR                      pszFile, 
    DWORD                       dwFlags, 
    LPCDSMIXBINS                pMixBins, 
    LPDWORD                     pdwPacketSize,
    LPFNXMEDIAOBJECTCALLBACK    pfnCallback,
    LPVOID                      pvContext,
    LPDIRECTSOUNDBUFFER         pOutputBuffer, 
    LPDIRECTSOUNDSTREAM *       ppStream, 
    LPVOID *                    ppvAudioData,
    LPDWORD                     pdwAudioDataSize,
    LPDWORD                     pdwPacketCount
);

HRESULT
CreateStdStream
(
    DWORD                       dwFlags, 
    LPCWAVEFORMATEX             pwfxFormat,
    LPCDSMIXBINS                pMixBins, 
    LPDWORD                     pdwPacketSize,
    LPFNXMEDIAOBJECTCALLBACK    pfnCallback,
    LPVOID                      pvContext,
    LPDIRECTSOUNDBUFFER         pOutputBuffer, 
    LPDIRECTSOUNDSTREAM *       ppStream, 
    LPVOID *                    ppvAudioData,
    LPDWORD                     pdwAudioDataSize,
    LPDWORD                     pdwPacketCount
);

HRESULT
CreateQueuedStreamFromFile
(
    LPCSTR                  pszFile, 
    DWORD                   dwFlags, 
    LPCDSMIXBINS            pMixBins, 
    LPDWORD                 pdwPacketSize,
    HANDLE                  hCompletionEvent,
    LPDIRECTSOUNDBUFFER     pOutputBuffer, 
    LPDIRECTSOUNDSTREAM *   ppStream, 
    LPVOID *                ppvAudioData
);

HRESULT
CreateStdQueuedStream
(
    DWORD                   dwFlags, 
    LPCWAVEFORMATEX         pwfxFormat,
    LPCDSMIXBINS            pMixBins, 
    LPDWORD                 pdwPacketSize,
    HANDLE                  hCompletionEvent,
    LPDIRECTSOUNDBUFFER     pOutputBuffer, 
    LPDIRECTSOUNDSTREAM *   ppStream, 
    LPVOID *                ppvAudioData
);

//
// Stream utilties
//

HRESULT
PauseStream
(
    LPDIRECTSOUNDSTREAM     pStream,
    BOOL                    fPause  = TRUE
);

HRESULT
FlushStream
(
    LPDIRECTSOUNDSTREAM     pStream,
    DWORD                   dwFlags = 0,
    DWORD                   dwDelay = 0,
    BOOL                    fWait   = FALSE
);

HRESULT
WaitStream
(
    LPDIRECTSOUNDSTREAM     pStream,
    BOOL                    fPlaying
);

void CALLBACK
ResubmitPacketCallback
(
    LPVOID                  pvStreamContext,
    LPVOID                  pvPacketContext,
    DWORD                   dwStatus
);

//
// Generic helpers
//

void
GenerateTone
(
    LPVOID                      pvBuffer,
    DWORD                       dwBufferBytes,
    DWORD                       dwPlaybackFrequency,
    DWORD                       dwBitsPerSample,
    DWORD                       dwChannelCount,
    DWORD                       dwToneFrequency
);

void
GenerateTone8
(
    LPVOID                      pvBuffer,
    DWORD                       dwBufferBytes,
    DWORD                       dwPlaybackFrequency,
    DWORD                       dwChannelCount,
    DWORD                       dwToneFrequency
);

void
GenerateTone16
(
    LPVOID                      pvBuffer,
    DWORD                       dwBufferBytes,
    DWORD                       dwPlaybackFrequency,
    DWORD                       dwChannelCount,
    DWORD                       dwToneFrequency
);

void
GenerateNoise
(
    LPVOID                  pvBuffer,
    DWORD                   dwBufferSize
);

HRESULT
MakeMultiChannel
(
    LPCWAVEFORMATEX         pwfxSrc,
    LPWAVEFORMATEX          pwfxDst,
    LPCVOID                 pvSrcData,
    DWORD                   dwSrcDataSize,
    LPVOID *                ppvDstData,
    LPDWORD                 pdwDstDataSize
);

LPCWAVEFORMATEX
CreateFormat
(
    DWORD                   dwFormatTag,
    DWORD                   dwSamplesPerSec,
    DWORD                   dwBitsPerSample,
    DWORD                   dwChannels
);

HRESULT
LoadEffectsImage
(
    LPCSTR                  pszImageName,
    DWORD                   dwFlags
);

HRESULT
GetDirectSoundCaps
(
    LPDSCAPS                pdsc
);

#endif // __cplusplus

#endif // __UTIL_H__
