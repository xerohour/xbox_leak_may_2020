/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       testds.cpp
 *  Content:    DirectSound test functions.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  06/08/00    dereks  Created.
 *
 ****************************************************************************/

#include "testds.h"
#include "pcm.h"
#include "adpcm.h"
#include "compress.h"
#include "8bit.h"
#include "pcmaiff.h"
#include "loop.h"
#include "mixed.h"

#ifdef DEBUG

EXTERN_C BOOL g_fDirectSoundSimulateDeltaPanic;

#endif // DEBUG

LPDIRECTSOUND g_pDirectSound = NULL;


/****************************************************************************
 *
 *  RotateVectorAzimuth
 *
 *  Description:
 *      Rotates a vector in a circle or elipse around a point.
 *
 *  Arguments:
 *      FLOAT [in]: circle size in radians.
 *      FLOAT [in]: theta in radians.
 *      LPD3DXVECTOR3 [in/out]: vector.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "RotateVectorAzimuth"

__inline void
RotateVectorAzimuth
(
    FLOAT                   flDistance,
    FLOAT                   flTheta,
    LPD3DVECTOR             pvrPosition
)
{
    pvrPosition->x = (float)cos(flTheta) * flDistance;
    pvrPosition->z = (float)sin(flTheta) * flDistance;
}


/****************************************************************************
 *
 *  Test3d
 *
 *  Description:
 *      Moves a sound source around a circle.
 *
 *  Arguments:
 *      FLOAT [in]: listener position (X).
 *      FLOAT [in]: listener position (Y).
 *      FLOAT [in]: listener position (Z).
 *      FLOAT [in]: source circle radians.
 *      FLOAT [in]: source angular velocity.
 *      DWORD [in]: time, in ms, to run the test.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "Test3d"

HRESULT
Test3d
(
    FLOAT                   flListenerPositionX,
    FLOAT                   flListenerPositionY,
    FLOAT                   flListenerPositionZ,
    FLOAT                   flDistance,
    FLOAT                   flAngularVelocity,
    DWORD                   dwTime
)
{
    static const LPCSTR     pszFile                 = "t:\\media\\audio\\pcm\\heli.wav";
    LPDIRECTSOUNDBUFFER     pBuffer                 = NULL;
    INT                     nLastX                  = -1;
    INT                     nLastY                  = -1;
    INT                     nLastZ                  = -1;
    DSMIXBINVOLUMEPAIR      aMixBinVolumePairs[]    = { DSMIXBINVOLUMEPAIRS_REQUIRED_3D, { DSMIXBIN_FRONT_CENTER, 0 } };
    DSMIXBINS               MixBins;
    FLOAT                   flTheta;
    D3DXVECTOR3             vrPreviousPosition;
    D3DXVECTOR3             vrCurrentPosition;
    D3DXVECTOR3             vrVelocity;
    DWORD                   dwStartTime;
    DWORD                   dwCurrentTime;
    FLOAT                   flTimeDelta;
    HRESULT                 hr;

    MixBins.dwMixBinCount = NUMELMS(aMixBinVolumePairs);
    MixBins.lpMixBinVolumePairs = aMixBinVolumePairs;

    //
    // Convert angular velocity to radians
    //

    flAngularVelocity = (flAngularVelocity * D3DX_PI) / 180.0f;
    flTheta = (90.0f * D3DX_PI) / 180.0f;

    //
    // Calculate the buffer's initial position
    //

    vrPreviousPosition.x = 0;
    vrPreviousPosition.y = 0;
    vrPreviousPosition.z = flDistance;

    vrCurrentPosition = vrPreviousPosition;

    //
    // Set the listener position
    //
    
    hr = ValidateResult(g_pDirectSound->SetPosition(flListenerPositionX, flListenerPositionY, flListenerPositionZ, DS3D_IMMEDIATE));

    //
    // Create the buffer
    //

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(CreateBufferFromFile(pszFile, DSBCAPS_CTRL3D, &MixBins, NULL, &pBuffer));
    }

    //
    // Set the buffer's initial position
    //

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(pBuffer->SetPosition(vrPreviousPosition.x, vrPreviousPosition.y, vrPreviousPosition.z, DS3D_IMMEDIATE));
    }

    //
    // Start the buffer playing
    //

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(PlayBuffer(pBuffer, DSBPLAY_LOOPING));
    }

    //
    // Get the current system time
    //

    if(SUCCEEDED(hr))
    {
        dwStartTime = GetTickCount();
    }

    //
    // Enter the main loop
    // 

    while(SUCCEEDED(hr) && !fBreakTest)
    {
        //
        // Get the current system time
        //

        while(dwStartTime == (dwCurrentTime = GetTickCount()));

        if(INFINITE != dwTime)
        {
            if(dwCurrentTime >= dwStartTime + dwTime)
            {
                break;
            }
        }

        //
        // Calculate the distance the sound should travel
        //

        flTimeDelta = (FLOAT)(dwCurrentTime - dwStartTime) / 1000.0f;
        flTheta += flAngularVelocity * flTimeDelta;

        //
        // Set the object's new position and velocity
        //

        RotateVectorAzimuth(flDistance, flTheta, &vrCurrentPosition);

        vrVelocity.x = (vrCurrentPosition.x - vrPreviousPosition.x) / flTimeDelta;
        vrVelocity.y = (vrCurrentPosition.y - vrPreviousPosition.y) / flTimeDelta;
        vrVelocity.z = (vrCurrentPosition.z - vrPreviousPosition.z) / flTimeDelta;

        hr = ValidateResult(pBuffer->SetVelocity(vrVelocity.x, vrVelocity.y, vrVelocity.z, DS3D_DEFERRED));

        if(SUCCEEDED(hr))
        {
            hr = ValidateResult(pBuffer->SetPosition(vrCurrentPosition.x, vrCurrentPosition.y, vrCurrentPosition.z, DS3D_DEFERRED));
        }

        if(SUCCEEDED(hr))
        {
            hr = ValidateResult(g_pDirectSound->CommitDeferredSettings());
        }

        if(SUCCEEDED(hr))
        {
            if(((INT)vrCurrentPosition.x != nLastX) || ((INT)vrCurrentPosition.y != nLastY) || ((INT)vrCurrentPosition.z != nLastZ))
            {
                nLastX = (INT)vrCurrentPosition.x;
                nLastY = (INT)vrCurrentPosition.y;
                nLastZ = (INT)vrCurrentPosition.z;

                DPF_TEST("%d %d %d (%d %d %d)", nLastX, nLastY, nLastZ, (int)vrVelocity.x, (int)vrVelocity.y, (int)vrVelocity.z);
            }
        }

        //
        // Save settings for the next time around
        //

        vrPreviousPosition = vrCurrentPosition;
        dwStartTime = dwCurrentTime;
    }

    //
    // Clean up
    //

    RELEASE(pBuffer);

    return hr;
}


/****************************************************************************
 *
 *  PlayOneShotBuffer
 *
 *  Description:
 *      Plays a single buffer to completion.
 *
 *  Arguments:
 *      LPCSTR [in]: file path.
 *      DWORD [in]: buffer creation flags.
 *
 *  Returns:  
 *      HRESULT: COM result code
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "PlayOneShotBuffer"

HRESULT
PlayOneShotBuffer
(
    LPCSTR                  pszFile, 
    DWORD                   dwFlags     = 0,
    LPCDSMIXBINS            pMixBins    = NULL
)
{
    LPDIRECTSOUNDBUFFER     pBuffer     = NULL;
    HRESULT                 hr;

    hr = ValidateResult(CreateBufferFromFile(pszFile, dwFlags, pMixBins, NULL, &pBuffer));

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(PlayBufferToCompletion(pBuffer));
    }

    RELEASE(pBuffer);

    return hr;
}


/****************************************************************************
 *
 *  PlayOneShotBuffer
 *
 *  Description:
 *      Plays a single buffer to completion.
 *
 *  Arguments:
 *      DWORD [in]: buffer creation flags.
 *      DWORD [in]: format tag.
 *      DWORD [in]: sampling rate.
 *      DWORD [in]: bit resolution.
 *      DWORD [in]: channel count.
 *
 *  Returns:  
 *      HRESULT: COM result code
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "PlayOneShotBuffer"

HRESULT
PlayOneShotBuffer
(
    DWORD                   dwFlags,
    DWORD                   dwFormatTag,
    DWORD                   dwSamplesPerSec,
    DWORD                   dwBitsPerSample,
    DWORD                   dwChannelCount,
    LPCDSMIXBINS            pMixBins            = NULL
)
{
    WAVEFORMATEX            wfxFormat           = INIT_PCM_WAVEFORMAT(dwChannelCount, dwSamplesPerSec, dwBitsPerSample);
    LPCSTR                  pszFileName         = GetStdWaveFile(&wfxFormat);
    
    return PlayOneShotBuffer(pszFileName, dwFlags, pMixBins);
}


/****************************************************************************
 *
 *  TestSequentialBuffers
 *
 *  Description:
 *      Plays a list of secondary buffers sequentially.
 *
 *  Arguments:
 *      DWORD [in]: format tag.
 *
 *  Returns:  
 *      HRESULT: COM result code
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestSequentialBuffers"

HRESULT
TestSequentialBuffers
(
    DWORD                   dwFormatTag
)
{
    static const DWORD      adwSampleRates[]    = { 8000, 11025, 16000, 22050, 32000, 44100, 48000 };
    static const DWORD      adwChannelCounts[]  = { 1, 2 };
    static const DWORD      adwBitResolutions[] = { 8, 16 };
    static const DWORD      dwBufferCount       = NUMELMS(adwSampleRates) * NUMELMS(adwChannelCounts) * NUMELMS(adwBitResolutions);
    LPDIRECTSOUNDBUFFER     pBuffer             = NULL;
    HRESULT                 hr                  = DS_OK;
    DWORD                   i;

    for(i = 0; (i < dwBufferCount) && SUCCEEDED(hr); i++)
    {
        hr = PlayOneShotBuffer(0, dwFormatTag, adwSampleRates[i % NUMELMS(adwSampleRates)], adwBitResolutions[i % NUMELMS(adwBitResolutions)], adwChannelCounts[i % NUMELMS(adwChannelCounts)]);
    }

    return hr;
}


/****************************************************************************
 *
 *  TestMultipleBuffers
 *
 *  Description:
 *      Plays multiple buffers of varying formats.
 *
 *  Arguments:
 *      DWORD [in]: format tag.
 *
 *  Returns:  
 *      HRESULT: COM result code
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestMultipleBuffers"

HRESULT
TestMultipleBuffers
(
    DWORD                   dwFormatTag
)
{
    static const DWORD      adwSampleRates[]            = { 8000, 11025, 16000, 22050, 32000, 44100, 48000 };
    static const DWORD      adwChannelCounts[]          = { 1, 2 };
    static const DWORD      adwBitResolutions[]         = { 8, 16 };
    static const DWORD      dwBufferCount               = NUMELMS(adwSampleRates) * NUMELMS(adwChannelCounts) * NUMELMS(adwBitResolutions);
    LPDIRECTSOUNDBUFFER     apBuffers[dwBufferCount]    = { NULL };
    DWORD                   dwSampleRateIndex           = 0;
    DWORD                   dwChannelCountIndex         = 0;
    DWORD                   dwBitResolutionIndex        = 0;
    DWORD                   dwActiveBuffers             = 0;
    HRESULT                 hr                          = DS_OK;
    DWORD                   dwFreeSGEs;
    DWORD                   dwMaxBufferSize;
    DWORD                   dwBufferSize;
    LPCWAVEFORMATEX         pwfx;
    DWORD                   dwStatus;
    DWORD                   i;

    //
    // Get the count of free SGEs
    //

    hr = GetFreeSGEs(&dwFreeSGEs);

    //
    // Calculate the maximum buffer size
    //

    if(SUCCEEDED(hr))
    {
        dwMaxBufferSize = PAGE_SIZE;
    }

    //
    // Enter the creation loop
    //
    
    while(SUCCEEDED(hr) && (dwActiveBuffers < dwBufferCount))
    {
        //
        // Create the buffer
        //
        
        pwfx = CreateFormat(dwFormatTag, adwSampleRates[dwSampleRateIndex], adwBitResolutions[dwBitResolutionIndex], adwChannelCounts[dwChannelCountIndex]);
        dwBufferSize = dwMaxBufferSize;
        
        hr = CreateStdBuffer(0, pwfx, 0, NULL, &apBuffers[dwActiveBuffers], &dwBufferSize);

        if(SUCCEEDED(hr))
        {
            hr = PlayBuffer(apBuffers[dwActiveBuffers]);
        }

        if(SUCCEEDED(hr))
        {
            dwSampleRateIndex = (dwSampleRateIndex + 1) % NUMELMS(adwSampleRates);
            dwChannelCountIndex = (dwChannelCountIndex + 1) % NUMELMS(adwChannelCounts);
            dwBitResolutionIndex = (dwBitResolutionIndex + 1) % NUMELMS(adwBitResolutions);

            dwActiveBuffers++;
        }
    }

    //
    // Wait for all buffers to finish playing
    //

    while(SUCCEEDED(hr) && dwActiveBuffers)
    {
        DirectSoundDoWork();
        
        for(i = 0; (i < dwBufferCount) && SUCCEEDED(hr); i++)
        {
            if(apBuffers[i])
            {
                hr = ValidateResult(apBuffers[i]->GetStatus(&dwStatus));

                if(SUCCEEDED(hr) && !(dwStatus & DSBSTATUS_PLAYING))
                {
                    RELEASE(apBuffers[i]);

                    dwActiveBuffers--;
                }
            }
        }
    }

    return hr;
}


/****************************************************************************
 *
 *  TestMultipleStreams
 *
 *  Description:
 *      Plays multiple streams of varying formats.
 *
 *  Arguments:
 *      DWORD [in]: format tag.
 *      DWORD [in]: packet size, in bytes.
 *
 *  Returns:  
 *      HRESULT: COM result code
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestMultipleStreams"

HRESULT
TestMultipleStreams
(
    DWORD                   dwFormatTag,
    DWORD                   dwPacketSize
)
{
    static const DWORD      adwSampleRates[]                    = { 8000, 11025, 16000, 22050, 32000, 44100, 48000 };
    static const DWORD      adwChannelCounts[]                  = { 1, 2 };
    static const DWORD      adwBitResolutions[]                 = { 8, 16 };
    static const DWORD      dwStreamCount                       = NUMELMS(adwSampleRates) * NUMELMS(adwChannelCounts) * NUMELMS(adwBitResolutions);
    LPDIRECTSOUNDSTREAM     apStreams[dwStreamCount]            = { NULL };
    LPVOID                  apvStreamData[dwStreamCount]        = { NULL };
    HANDLE                  ahCompletionEvents[dwStreamCount]   = { NULL };
    DWORD                   dwSampleRateIndex                   = 0;
    DWORD                   dwChannelCountIndex                 = 0;
    DWORD                   dwBitResolutionIndex                = 0;
    DWORD                   dwActiveStreams                     = 0;
    HRESULT                 hr                                  = DS_OK;
    LPCWAVEFORMATEX         pwfx;
    DWORD                   i;

    //
    // Enter the creation loop
    //
    
    while(SUCCEEDED(hr) && (dwActiveStreams < dwStreamCount))
    {
        hr = ValidatePtr(ahCompletionEvents[dwActiveStreams] = CreateEvent(NULL, FALSE, FALSE, NULL));

        if(SUCCEEDED(hr))
        {
            pwfx = CreateFormat(dwFormatTag, adwSampleRates[dwSampleRateIndex], adwBitResolutions[dwBitResolutionIndex], adwChannelCounts[dwChannelCountIndex]);

            dwPacketSize /= pwfx->nBlockAlign;
            dwPacketSize *= pwfx->nBlockAlign;
            
            hr = CreateStdQueuedStream(0, pwfx, 0, &dwPacketSize, ahCompletionEvents[dwActiveStreams], NULL, &apStreams[dwActiveStreams], &apvStreamData[dwActiveStreams]);
        }

        if(SUCCEEDED(hr))
        {
            hr = PauseStream(apStreams[dwActiveStreams], FALSE);
        }

        if(SUCCEEDED(hr))
        {
            dwSampleRateIndex = (dwSampleRateIndex + 1) % NUMELMS(adwSampleRates);
            dwChannelCountIndex = (dwChannelCountIndex + 1) % NUMELMS(adwChannelCounts);
            dwBitResolutionIndex = (dwBitResolutionIndex + 1) % NUMELMS(adwBitResolutions);

            dwActiveStreams++;
        }
    }

    //
    // Wait for each stream to complete
    //

    while(SUCCEEDED(hr) && dwActiveStreams)
    {
        DirectSoundDoWork();
        
        if((i = WaitForMultipleObjects(dwStreamCount, ahCompletionEvents, FALSE, 0)) < dwStreamCount)
        {
            RELEASE(apStreams[i]);
            FREE(apvStreamData[i]);

            dwActiveStreams--;
        }
    }

    //
    // Clean up
    //

    for(i = 0; i < dwStreamCount; i++)
    {
        CLOSE_HANDLE(ahCompletionEvents[i]);
    }

    return hr;
}


/****************************************************************************
 *
 *  TestNotifies
 *
 *  Description:
 *      Tests notifications.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestNotifies"

HRESULT
TestNotifies
(
    DWORD                   dwFormatTag,
    DWORD                   dwSampleRate,
    DWORD                   dwBitsPerSample,
    DWORD                   dwChannelCount,
    DWORD                   dwNotifyCount,
    BOOL                    fLoop
)
{
    LPCWAVEFORMATEX         pwfxFormat      = CreateFormat(dwFormatTag, dwSampleRate, dwBitsPerSample, dwChannelCount);
    LPXFILEMEDIAOBJECT      pWaveFile       = NULL;
    LPDIRECTSOUNDBUFFER     pBuffer         = NULL;
    DWORD                   dwVolumeIndex   = 0;
    DWORD                   dwDuration      = 0;
    DWORD                   dwPeriod;
    LPHANDLE                pahEvents;
    LPDSBPOSITIONNOTIFY     padsbpn;
    DSENVELOPEDESC          dseg;
    HRESULT                 hr;
    DWORD                   i;

    //
    // Create the buffer
    //

    hr = CreateStdBuffer(DSBCAPS_CTRLPOSITIONNOTIFY, pwfxFormat, 0, NULL, &pBuffer, &dwDuration);

    //
    // Calculate the notification period
    //
    
    if(SUCCEEDED(hr))
    {
        dwPeriod = dwDuration / (dwNotifyCount - 1) / pwfxFormat->nBlockAlign * pwfxFormat->nBlockAlign;
    }

    //
    // Create notification events
    //
    
    if(SUCCEEDED(hr))
    {
        hr = MALLOC(pahEvents, HANDLE, dwNotifyCount);
    }

    if(SUCCEEDED(hr))
    {
        for(i = 0; i < dwNotifyCount; i++)
        {
            if(FAILED(hr = ValidatePtr(pahEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL))))
            {
                break;
            }
        }
    }

    //
    // Set notifications
    //

    if(SUCCEEDED(hr))
    {
        hr = MALLOC(padsbpn, DSBPOSITIONNOTIFY, dwNotifyCount);
    }

    if(SUCCEEDED(hr))
    {
        for(i = 0; i < dwNotifyCount - 2; i++)
        {
            padsbpn[i].dwOffset = i * dwPeriod;
            padsbpn[i].hEventNotify = pahEvents[i];
        }

        padsbpn[i].dwOffset = dwDuration - pwfxFormat->nBlockAlign;
        padsbpn[i].hEventNotify = pahEvents[i];

        i++;
        
        padsbpn[i].dwOffset = DSBPN_OFFSETSTOP;
        padsbpn[i].hEventNotify = pahEvents[i];

        hr = ValidateResult(pBuffer->SetNotificationPositions(dwNotifyCount, padsbpn));
    }

    //
    // Start playback
    //

    if(SUCCEEDED(hr))
    {
        hr = PlayBuffer(pBuffer, fLoop ? DSBPLAY_LOOPING : 0);
    }

    //
    // Wait for playback to complete
    //

    if(SUCCEEDED(hr))
    {
        while(!fBreakTest)
        {
            DirectSoundDoWork();
            
            if((i = WaitForMultipleObjects(dwNotifyCount, pahEvents, FALSE, 100)) < dwNotifyCount)
            {
                DPF_TEST("Event %lu signaled", i);

                if(WAIT_OBJECT_0 + dwNotifyCount - 1 == i)
                {
                    break;
                }
            }
        }
    }

    //
    // Clean up
    //

    if(pahEvents)
    {
        for(i = 0; i < dwNotifyCount; i++)
        {
            CLOSE_HANDLE(pahEvents[i]);
        }
    }

    FREE(pahEvents);
    FREE(padsbpn);

    RELEASE(pBuffer);
    RELEASE(pWaveFile);

    return hr;
}


/****************************************************************************
 *
 *  PlayOneShotStream
 *
 *  Description:
 *      Tests streaming.
 *
 *  Arguments:
 *      LPCSTR [in]: file path.
 *      DWORD [in]: stream creation flags.
 *      DWORD [in]: packet size, in bytes.
 *      DWORD [in]: mixbin mask.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "PlayOneShotStream"

HRESULT
PlayOneShotStream
(
    LPCSTR                  pszFile,
    DWORD                   dwFlags,
    DWORD                   dwPacketSize,
    LPCDSMIXBINS            pMixBins            = NULL 
)
{
    LPDIRECTSOUNDSTREAM     pStream             = NULL;
    LPVOID                  pvDataBuffer        = NULL;
    HANDLE                  hCompletionEvent;
    HRESULT                 hr;
    
    //
    // Create the completion event
    //
    
    hr = ValidatePtr(hCompletionEvent = CreateEvent(NULL, FALSE, FALSE, NULL));

    //
    // Create the stream 
    //

    if(SUCCEEDED(hr))
    {
        hr = CreateQueuedStreamFromFile(pszFile, dwFlags, pMixBins, &dwPacketSize, hCompletionEvent, NULL, &pStream, &pvDataBuffer);
    }

    //
    // Start playback
    //

    if(SUCCEEDED(hr))
    {
        hr = PauseStream(pStream, FALSE);
    }

    //
    // Wait for the completion event to be signaled
    //

    if(SUCCEEDED(hr))
    {
        while(TRUE)
        {
            DirectSoundDoWork();

            if(WAIT_TIMEOUT != WaitForSingleObject(hCompletionEvent, 0))
            {
                break;
            }
        }
    }

    //
    // Clean up
    //

    RELEASE(pStream);
    FREE(pvDataBuffer);
    CLOSE_HANDLE(hCompletionEvent);

    return hr;
}


/****************************************************************************
 *
 *  PlayOneShotStream
 *
 *  Description:
 *      Tests streaming.
 *
 *  Arguments:
 *      DWORD [in]: stream creation flags.
 *      DWORD [in]: format tag.
 *      DWORD [in]: sampling rate.
 *      DWORD [in]: bit resolution.
 *      DWORD [in]: channel count.
 *      DWORD [in]: packet size, in bytes.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "PlayOneShotStream"

HRESULT
PlayOneShotStream
(
    DWORD                   dwFlags,
    DWORD                   dwFormatTag,
    DWORD                   dwSamplesPerSec,
    DWORD                   dwBitsPerSample,
    DWORD                   dwChannelCount,
    DWORD                   dwPacketSize,
    LPCDSMIXBINS            pMixBins        = NULL
)
{
    LPCWAVEFORMATEX         pwfxFormat      = CreateFormat(dwFormatTag, dwSamplesPerSec, dwBitsPerSample, dwChannelCount);
    LPCSTR                  pszFileName     = GetStdWaveFile(pwfxFormat);
    
    return PlayOneShotStream(pszFileName, dwFlags, dwPacketSize, pMixBins);
}


/****************************************************************************
 *
 *  TestBufferOffsets
 *
 *  Description:
 *      Tests buffer play region.
 *
 *  Arguments:
 *      DWORD [in]: format tag.
 *      DWORD [in]: sampling rate.
 *      DWORD [in]: bit resolution.
 *      DWORD [in]: channel count.
 *      FLOAT [in]: play start position (0 to 1).
 *      FLOAT [in]: play end position (0 to 1).
 *      FLOAT [in]: loop start position (0 to 1).
 *      FLOAT [in]: loop end position (0 to 1).
 *      BOOL [in]: TRUE to play looping.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "TestBufferOffsets"

HRESULT
TestBufferOffsets
(
    DWORD                   dwFormatTag,
    DWORD                   dwSampleRate,
    DWORD                   dwBitsPerSample,
    DWORD                   dwChannelCount,
    FLOAT                   flPlayStart,
    FLOAT                   flPlayEnd,
    FLOAT                   flLoopStart,
    FLOAT                   flLoopEnd,
    BOOL                    fLoop
)
{
    LPCWAVEFORMATEX         pwfxFormat  = CreateFormat(dwFormatTag, dwSampleRate, dwBitsPerSample, dwChannelCount);
    LPDIRECTSOUNDBUFFER     pBuffer     = NULL;
    DWORD                   dwDuration  = 0;
    DWORD                   dwPlayStart;
    DWORD                   dwPlayLength;
    DWORD                   dwLoopStart;
    DWORD                   dwLoopLength;
    HRESULT                 hr;

    hr = CreateStdBuffer(0, pwfxFormat, 0, NULL, &pBuffer, &dwDuration);

    if(SUCCEEDED(hr))
    {
        dwPlayStart = (DWORD)((FLOAT)dwDuration * flPlayStart) / pwfxFormat->nBlockAlign * pwfxFormat->nBlockAlign;
        dwPlayLength = ((DWORD)((FLOAT)dwDuration * flPlayEnd) / pwfxFormat->nBlockAlign * pwfxFormat->nBlockAlign) - dwPlayStart;
        
        dwLoopStart = (DWORD)((FLOAT)dwDuration * flLoopStart) / pwfxFormat->nBlockAlign * pwfxFormat->nBlockAlign - dwPlayStart;
        dwLoopLength = ((DWORD)((FLOAT)dwDuration * flLoopEnd) / pwfxFormat->nBlockAlign * pwfxFormat->nBlockAlign) - dwLoopStart - dwPlayStart;

        DPF_TEST("Setting buffer play region to %lu, %lu and loop region to %lu, %lu (duration %lu)", dwPlayStart, dwPlayLength, dwLoopStart, dwLoopLength, dwDuration);
        
        hr = ValidateResult(pBuffer->SetPlayRegion(dwPlayStart, dwPlayLength));

        if(SUCCEEDED(hr))
        {
            hr = ValidateResult(pBuffer->SetLoopRegion(dwLoopStart, dwLoopLength));
        }
    }

    if(SUCCEEDED(hr))
    {
        hr = PlayBuffer(pBuffer, fLoop ? DSBPLAY_LOOPING : 0);
    }

    if(SUCCEEDED(hr))
    {
        Sleep(30000);

        hr = PlayBuffer(pBuffer);
    }

    if(SUCCEEDED(hr))
    {
        hr = WaitBuffer(pBuffer, FALSE);
    }

    RELEASE(pBuffer);

    return hr;
}


/****************************************************************************
 *
 *  TestSubMix
 *
 *  Description:
 *      Tests basic submixing.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to test MIXIN.  FALSE to test FXIN.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "TestSubMix"

HRESULT
TestSubMix
(
    BOOL                    fMixIn,
    DWORD                   dwFormatTag
)
{
    static const DWORD      adwSampleRates[]            = { /*22050, 32000, */44100, 48000};
    static const DWORD      adwChannelCounts[]          = { 1 };
    static const DWORD      adwBitResolutions[]         = { 16 };
    static const DWORD      dwBufferCount               = NUMELMS(adwSampleRates) * NUMELMS(adwChannelCounts) * NUMELMS(adwBitResolutions);
    LPDIRECTSOUNDBUFFER     apBuffers[dwBufferCount]    = { NULL };
    LPDIRECTSOUNDBUFFER     apOutputBuffers[2]          = { NULL };
    DWORD                   dwActiveBuffers             = 0;
    DWORD                   dwSampleRateIndex           = 0;
    DWORD                   dwChannelCountIndex         = 0;
    DWORD                   dwBitResolutionIndex        = 0;
    HRESULT                 hr                          = DS_OK;
    DSBUFFERDESC            dsbd;
    DWORD                   dwFreeSGEs;
    DWORD                   dwMaxBufferSize;
    DWORD                   dwBufferSize;
    DWORD                   dwStatus;
    LPCWAVEFORMATEX         pwfx;
    DSMIXBINS               MixBins;
    DWORD                   i, z;

    //
    // Create output buffers
    //

    ZeroMemory(&dsbd, sizeof(dsbd));

    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = fMixIn ? DSBCAPS_MIXIN : DSBCAPS_FXIN;

    for(i = 0; (i < NUMELMS(apOutputBuffers)) && SUCCEEDED(hr); i++)
    {
        if(!fMixIn)
        {
            dsbd.dwInputMixBin = DSMIXBIN_FXSEND_10 + i;
        }
        
        hr = ValidateResult(DirectSoundCreateBuffer(&dsbd, &apOutputBuffers[i]));
    }

    //
    // Get the count of free SGEs
    //

    if(SUCCEEDED(hr))
    {
        hr = GetFreeSGEs(&dwFreeSGEs);
    }

    //
    // Calculate the maximum buffer size
    //

    if(SUCCEEDED(hr))
    {
        dwMaxBufferSize = ((dwFreeSGEs - dwBufferCount) * PAGE_SIZE) / dwBufferCount / 4 * 4;
    }

    //
    // Enter the creation loop
    //
    
    while(SUCCEEDED(hr) && (dwActiveBuffers < dwBufferCount))
    {
        //
        // Create the buffer
        //
        
        pwfx = CreateFormat(dwFormatTag, adwSampleRates[dwSampleRateIndex], adwBitResolutions[dwBitResolutionIndex], adwChannelCounts[dwChannelCountIndex]);
        dwBufferSize = dwMaxBufferSize;

        MixBins.dwMixBinCount = 0;
        
        hr = CreateStdBuffer(0, pwfx, &MixBins, apOutputBuffers[dwActiveBuffers % NUMELMS(apOutputBuffers)], &apBuffers[dwActiveBuffers], &dwBufferSize);

        if(SUCCEEDED(hr))
        {
            hr = PlayBuffer(apBuffers[dwActiveBuffers]);
        }

        if(SUCCEEDED(hr))
        {
            dwSampleRateIndex = (dwSampleRateIndex + 1) % NUMELMS(adwSampleRates);
            dwChannelCountIndex = (dwChannelCountIndex + 1) % NUMELMS(adwChannelCounts);
            dwBitResolutionIndex = (dwBitResolutionIndex + 1) % NUMELMS(adwBitResolutions);

            dwActiveBuffers++;
        }
    }

    //
    // Wait for all buffers to finish playing
    //

    while(SUCCEEDED(hr) && dwActiveBuffers)
    {
        for(i = 0; (i < dwBufferCount) && SUCCEEDED(hr); i++)
        {
            if(apBuffers[i])
            {
                hr = ValidateResult(apBuffers[i]->GetStatus(&dwStatus));

                if(SUCCEEDED(hr) && !(dwStatus & DSBSTATUS_PLAYING))
                {
                    RELEASE(apBuffers[i]);

                    dwActiveBuffers--;
                }
            }
        }
    }

    //
    // Release output buffers
    //

    for(i = 0; i < NUMELMS(apOutputBuffers); i++)
    {
        RELEASE(apOutputBuffers[i]);
    }

    return hr;
}


/****************************************************************************
 *
 *  TestDLS
 *
 *  Description:
 *      Tests basic DLS functionality.
 *
 *  Arguments:
 *      DWORD [in]: format tag.
 *      DWORD [in]: sampling rate.
 *      DWORD [in]: bits per sample.
 *      DWORD [in]: channel count.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestDLS"

HRESULT
TestDLS
(
    DWORD                   dwFormatTag,
    DWORD                   dwSamplesPerSec,
    DWORD                   dwBitsPerSample,
    DWORD                   dwChannelCount
)
{
    static const DWORD      dwSegmentLength = 200;
    LPDIRECTSOUNDBUFFER     pBuffer         = NULL;
    DSENVELOPEDESC          env;
    HRESULT                 hr;

    hr = CreateStdBuffer(0, CreateFormat(dwFormatTag, dwSamplesPerSec, dwBitsPerSample, dwChannelCount), 0, NULL, &pBuffer);

    if(SUCCEEDED(hr))
    {
        env.dwEG = DSEG_AMPLITUDE;
        env.dwMode = DSEG_MODE_DELAY;
        env.dwDelay = dwSegmentLength;
        env.dwAttack = dwSegmentLength;
        env.dwHold = dwSegmentLength;
        env.dwDecay = dwSegmentLength;
        env.dwRelease = dwSegmentLength;
        env.dwSustain = 0xFF;
        env.lPitchScale = 0;
        env.lFilterCutOff = 0;

        hr = ValidateResult(pBuffer->SetEG(&env));
    }

    if(SUCCEEDED(hr))
    {
        hr = PlayBuffer(pBuffer, DSBPLAY_LOOPING);
    }

    if(SUCCEEDED(hr))
    {
        hr = StopBuffer(pBuffer, DSBSTOPEX_ENVELOPE, 10000, FALSE, TRUE);
    }

    RELEASE(pBuffer);

    return hr;
}


/****************************************************************************
 *
 *  TestStreamStop
 *
 *  Description:
 *      Tests stream release envelopes.
 *
 *  Arguments:
 *      DWORD [in]: flags.
 *      DWORD [in]: format tag.
 *      DWORD [in]: sampling rate.
 *      DWORD [in]: bits per sample.
 *      DWORD [in]: channel count.
 *      DWORD [in]: packet size, in bytes.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestStreamStop"

HRESULT
TestStreamStop
(
    DWORD                   dwFlags,
    DWORD                   dwFormatTag,
    DWORD                   dwSamplesPerSec,
    DWORD                   dwBitsPerSample,
    DWORD                   dwChannelCount,
    DWORD                   dwPacketSize
)
{
    static const DWORD      dwSegmentLength     = 200;
    static const DWORD      dwDelay             = 5000;
    const LPCWAVEFORMATEX   pwfxFormat          = CreateFormat(dwFormatTag, dwSamplesPerSec, dwBitsPerSample, dwChannelCount);
    LPDIRECTSOUNDSTREAM     pStream             = NULL;
    LPVOID                  pvAudioData         = NULL;
    DSENVELOPEDESC          env;
    HRESULT                 hr;

    hr = CreateStdQueuedStream(0, pwfxFormat, NULL, &dwPacketSize, NULL, NULL, &pStream, &pvAudioData);

    if(SUCCEEDED(hr))
    {
        ZeroMemory(&env, sizeof(env));
        
        env.dwEG = DSEG_AMPLITUDE;
        env.dwMode = DSEG_MODE_HOLD;
        env.dwRelease = dwSegmentLength;
        env.dwSustain = 0xFF;

        hr = ValidateResult(pStream->SetEG(&env));
    }

    if(SUCCEEDED(hr))
    {
        hr = PauseStream(pStream, FALSE);
    }

    if(SUCCEEDED(hr))
    {
        hr = FlushStream(pStream, dwFlags, (dwFlags & DSSTREAMFLUSHEX_ASYNC) ? dwDelay : 0, TRUE);
    }

    RELEASE(pStream);

    FREE(pvAudioData);

    return hr;
}


/****************************************************************************
 *
 *  TestSurroundEncoder
 *
 *  Description:
 *      Tests surround-sound encoding.
 *
 *  Arguments:
 *      DWORD [in]: format tag.
 *      DWORD [in]: sampling rate.
 *      DWORD [in]: bits per sample.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#if 0

#undef DPF_FNAME
#define DPF_FNAME "TestSurroundEncoder"

HRESULT
TestSurroundEncoder
(
    DWORD                       dwFormatTag,
    DWORD                       dwSamplesPerSec,
    DWORD                       dwBitsPerSample
)
{
    typedef struct
    {
        LPCSTR                  pszDescription;
        DWORD                   pMixBins;
    } TESTMIXBINS;

    static const TESTMIXBINS    aMixBins[] =
    {

#if 0

        { "All speakers",       DSMIXBIN_SPEAKER_MASK },
        { "Front left",         DSMIXBIN_FRONT_LEFT },
        { "Front right",        DSMIXBIN_FRONT_RIGHT },
        { "Front center",       DSMIXBIN_FRONT_CENTER },
        { "Back left",          DSMIXBIN_BACK_LEFT },
        { "Back right",         DSMIXBIN_BACK_RIGHT },
        { "LFE",                DSMIXBIN_LOW_FREQUENCY },

#else

        { "Front",              DSMIXBIN_FRONT_LEFT },
        { "Back",               DSMIXBIN_BACK_LEFT },

#endif

    };
    
    static const DWORD          dwDuration  = 5000;
    LPDIRECTSOUNDBUFFER         pBuffer     = NULL;
    HRESULT                     hr;
    DWORD                       i;

    hr = CreateStdBuffer(0, CreateFormat(dwFormatTag, dwSamplesPerSec, dwBitsPerSample, 1), 0, NULL, &pBuffer);

    for(i = 0; (i < NUMELMS(aMixBins)) && SUCCEEDED(hr); i++)
    {
        DPF_TEST(aMixBins[i].pszDescription);

        hr = ValidateResult(pBuffer->SetMixBins(aMixBins[i].pMixBins));

        if(SUCCEEDED(hr))
        {
            hr = PlayBuffer(pBuffer, DSBPLAY_LOOPING);
        }

        if(SUCCEEDED(hr))
        {
            Sleep(dwDuration);
        }
    }

    RELEASE(pBuffer);

    return hr;
}

#endif


/****************************************************************************
 *
 *  TestSgeSharing
 *
 *  Description:
 *      Tests whether buffer SGE sharing works by creating two very large
 *      buffers and pointing them to the same memory.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestSgeSharing"

HRESULT
TestSgeSharing
(
    void
)
{
    LPVOID                      pvBufferData    = NULL;
    LPDIRECTSOUNDBUFFER         apBuffers[2]    = { NULL };
    DWORD                       dwBufferSize;
    DSBUFFERDESC                dsbd;
    WAVEFORMATEX                wfx;
    HRESULT                     hr;
    DWORD                       i;

    XAudioCreatePcmFormat(2, 48000, 16, &wfx);

    ZeroMemory(&dsbd, sizeof(dsbd));

    dsbd.dwSize = sizeof(dsbd);
    dsbd.lpwfxFormat = &wfx;

    dwBufferSize = -1;
    
    hr = ClampBufferSize(&dwBufferSize);

    if(SUCCEEDED(hr))
    {
        hr = MALLOC(pvBufferData, BYTE, dwBufferSize);
    }

    if(SUCCEEDED(hr))
    {
        GenerateTone16(pvBufferData, dwBufferSize, wfx.nSamplesPerSec, wfx.nChannels, 500);
    }

    for(i = 0; (i < NUMELMS(apBuffers)) && SUCCEEDED(hr); i++)
    {
        hr = ValidateResult(DirectSoundCreateBuffer(&dsbd, &apBuffers[i]));

        if(SUCCEEDED(hr))
        {
            hr = ValidateResult(apBuffers[i]->SetBufferData(pvBufferData, dwBufferSize));
        }

        if(SUCCEEDED(hr))
        {
            hr = PlayBuffer(apBuffers[i], DSBPLAY_LOOPING);
        }

        if(SUCCEEDED(hr))
        {
            hr = StopBuffer(apBuffers[i], 0, (5000 * i) + 10000);
        }
    }

    for(i = 0; (i < NUMELMS(apBuffers)) && SUCCEEDED(hr); i++)
    {
        if(apBuffers[i])
        {
            hr = WaitBuffer(apBuffers[i], FALSE);

            RELEASE(apBuffers[i]);
        }
    }

    return hr;
}


/****************************************************************************
 *
 *  TestSgeHeap
 *
 *  Description:
 *      Allocates all available SGEs, then frees them all in a random order
 *      to verify that they were coalesced properly.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestSgeHeap"

HRESULT
TestSgeHeap
(
    void
)
{
    LPDIRECTSOUNDBUFFER     apBuffers[256]  = { NULL };
    LPVOID                  pvBufferData    = NULL;
    DWORD                   dwBufferCount;
    DWORD                   dwBufferSize;
    DWORD                   dwRemaining;
    DWORD                   dwFree;
    DSBUFFERDESC            dsbd;
    DSCAPS                  dsc;
    HRESULT                 hr;
    DWORD                   i;

    ZeroMemory(&dsbd, sizeof(dsbd));

    dsbd.dwSize = sizeof(dsbd);
    dsbd.lpwfxFormat = (LPWAVEFORMATEX)CreateFormat(WAVE_FORMAT_PCM, 44100, 16, 2);

    hr = GetDirectSoundCaps(&dsc);

    if(SUCCEEDED(hr))
    {
        dwBufferCount = dsc.dwFree2DBuffers + dsc.dwFree3DBuffers;
        dwBufferSize = dsc.dwFreeBufferSGEs / dwBufferCount;
    }

    if(SUCCEEDED(hr))
    {
        hr = ValidatePtr(pvBufferData = MmAllocateContiguousMemoryEx(dsc.dwFreeBufferSGEs * PAGE_SIZE, 0, 0xFFFFFFFF, PAGE_SIZE, PAGE_READWRITE));
    }

    if(SUCCEEDED(hr))
    {
        DPF_TEST("Creating %lu buffers", dwBufferCount);
        
        for(i = 0; (i < dwBufferCount) && SUCCEEDED(hr); i++)
        {
            hr = ValidateResult(DirectSoundCreateBuffer(&dsbd, &apBuffers[i]));

            if(SUCCEEDED(hr))
            {
                if(dwBufferCount - 1 == i)
                {
                    dwBufferSize = dsc.dwFreeBufferSGEs - dwBufferSize;
                }
                
                hr = ValidateResult(apBuffers[i]->SetBufferData(pvBufferData, dwBufferSize * PAGE_SIZE));
            }
        }
    }

    if(SUCCEEDED(hr))
    {
        hr = GetFreeSGEs(&dwFree);
    }

    if(SUCCEEDED(hr))
    {
        ASSERT(!dwFree);
    }

    if(SUCCEEDED(hr))
    {
        DPF_TEST("Releasing all buffers");

        for(dwRemaining = dwBufferCount; dwRemaining; dwRemaining--)
        {
            dwFree = rand() % dwRemaining;

            for(i = 0; i < dwBufferCount; i++)
            {
                if(apBuffers[i])
                {
                    if(!dwFree)
                    {
                        RELEASE(apBuffers[i]);
                        break;
                    }

                    dwFree--;
                }
            }

            ASSERT(!dwFree);
        }

        for(i = 0; i < dwBufferCount; i++)
        {
            ASSERT(!apBuffers[i]);
        }
    }

    if(SUCCEEDED(hr))
    {
        DPF_TEST("Creating big buffer");

        hr = ValidateResult(DirectSoundCreateBuffer(&dsbd, &apBuffers[0]));
    }

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(apBuffers[0]->SetBufferData(pvBufferData, dsc.dwFreeBufferSGEs * PAGE_SIZE));
    }

    RELEASE(apBuffers[0]);

    if(pvBufferData)
    {
        MmFreeContiguousMemory(pvBufferData);
    }

    return hr;
}


/****************************************************************************
 *
 *  Test256Voices
 *
 *  Description:
 *      Creates 256 voices.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "Test256Voices"

HRESULT
Test256Voices
(
    void
)
{
    static const DWORD          dwBufferSize    = PAGE_SIZE;
    LPVOID                      pvBufferData    = NULL;
    LPDIRECTSOUNDBUFFER         apBuffers[256]  = { NULL };
    DSCAPS                      dsc;
    DSBUFFERDESC                dsbd;
    WAVEFORMATEX                wfx;
    HRESULT                     hr;
    DWORD                       i;

    ZeroMemory(&dsbd, sizeof(dsbd));

    dsbd.dwSize = sizeof(dsbd);
    dsbd.lpwfxFormat = &wfx;

    XAudioCreatePcmFormat(1, dwBufferSize / 2, 16, &wfx);

    hr = MALLOC(pvBufferData, BYTE, dwBufferSize);

    if(SUCCEEDED(hr))
    {
        GenerateTone16(pvBufferData, dwBufferSize, wfx.nSamplesPerSec, wfx.nChannels, 500);
    }

    if(SUCCEEDED(hr))
    {
        hr = GetDirectSoundCaps(&dsc);
    }

    for(i = 0; (i < dsc.dwFree2DBuffers) && SUCCEEDED(hr); i++)
    {
        hr = ValidateResult(DirectSoundCreateBuffer(&dsbd, &apBuffers[i]));
    }

    if(SUCCEEDED(hr))
    {
        dsbd.dwFlags |= DSBCAPS_CTRL3D;
    }
    
    for(i = 0; (i < dsc.dwFree3DBuffers) && SUCCEEDED(hr); i++)
    {
        hr = ValidateResult(DirectSoundCreateBuffer(&dsbd, &apBuffers[dsc.dwFree2DBuffers + i]));
    }

    for(i = 0; (i < dsc.dwFree2DBuffers + dsc.dwFree3DBuffers) && SUCCEEDED(hr); i++)
    {
        hr = ValidateResult(apBuffers[i]->SetBufferData(pvBufferData, dwBufferSize));

        if(SUCCEEDED(hr))
        {
            hr = PlayBuffer(apBuffers[i], DSBPLAY_LOOPING);
        }
    }

    if(SUCCEEDED(hr))
    {
        Sleep(5000);
    }

    for(i = 0; i < NUMELMS(apBuffers); i++)
    {
        if(apBuffers[i])
        {
            StopBuffer(apBuffers[i], 0, 0, FALSE, TRUE);
        }
        
        RELEASE(apBuffers[i]);
    }

    FREE(pvBufferData);

    return hr;
}


/****************************************************************************
 *
 *  TestStop
 *
 *  Description:
 *      Tests stopping and restarting a buffer.
 *
 *  Arguments:
 *      DWORD [in]: format tag.
 *      DWORD [in]: sampling rate.
 *      DWORD [in]: bits per sample.
 *      DWORD [in]: channel count.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestStop"

HRESULT
TestStop
(
    DWORD                   dwFormatTag,
    DWORD                   dwSampleRate,
    DWORD                   dwBitsPerSample,
    DWORD                   dwChannelCount
)
{
    LPCWAVEFORMATEX         pwfxFormat  = CreateFormat(dwFormatTag, dwSampleRate, dwBitsPerSample, dwChannelCount);
    LPDIRECTSOUNDBUFFER     pBuffer     = NULL;
    DWORD                   dwDuration  = 0;
    DWORD                   dwPlay;
    DWORD                   dwWrite;
    HRESULT                 hr;

    hr = CreateStdBuffer(0, pwfxFormat, 0, NULL, &pBuffer, &dwDuration);

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(pBuffer->GetCurrentPosition(&dwPlay, &dwWrite));
    }

    if(SUCCEEDED(hr))
    {
        ASSERT(!dwPlay);
        ASSERT(!dwWrite);
    }

    if(SUCCEEDED(hr))
    {
        hr = PlayBuffer(pBuffer, DSBPLAY_LOOPING);
    }

    if(SUCCEEDED(hr))
    {
        Sleep(1);
        
        hr = ValidateResult(pBuffer->GetCurrentPosition(&dwPlay, &dwWrite));
    }

    if(SUCCEEDED(hr))
    {
        DPF_TEST("initial play:  play %lu write %lu duration %lu", dwPlay, dwWrite, dwDuration);
    }

    if(SUCCEEDED(hr))
    {
        Sleep(2000);

        hr = ValidateResult(pBuffer->SetCurrentPosition(dwDuration / 2 / pwfxFormat->nBlockAlign * pwfxFormat->nBlockAlign));
    }

    if(SUCCEEDED(hr))
    {
        Sleep(1);
        
        hr = ValidateResult(pBuffer->GetCurrentPosition(&dwPlay, &dwWrite));
    }

    if(SUCCEEDED(hr))
    {
        DPF_TEST("manual setposition:  play %lu write %lu duration %lu", dwPlay, dwWrite, dwDuration);
    }

    if(SUCCEEDED(hr))
    {
        hr = StopBuffer(pBuffer, 0, 10000, FALSE, TRUE);
    }

    if(SUCCEEDED(hr))
    {
        Sleep(2500);
        
        hr = ValidateResult(pBuffer->GetCurrentPosition(&dwPlay, &dwWrite));
    }

    if(SUCCEEDED(hr))
    {
        DPF_TEST("forced stop:  play %lu write %lu duration %lu", dwPlay, dwWrite, dwDuration);
    }

    if(SUCCEEDED(hr))
    {
        hr = PlayBuffer(pBuffer);
    }

    if(SUCCEEDED(hr))
    {
        Sleep(1);
        
        hr = ValidateResult(pBuffer->GetCurrentPosition(&dwPlay, &dwWrite));
    }

    if(SUCCEEDED(hr))
    {
        DPF_TEST("play from current position:  play %lu write %lu duration %lu", dwPlay, dwWrite, dwDuration);
    }

    if(SUCCEEDED(hr))
    {
        hr = WaitBuffer(pBuffer, FALSE);
    }

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(pBuffer->GetCurrentPosition(&dwPlay, &dwWrite));
    }

    if(SUCCEEDED(hr))
    {
        DPF_TEST("natural stop:  play %lu write %lu duration %lu", dwPlay, dwWrite, dwDuration);
    }

    if(SUCCEEDED(hr))
    {
        hr = PlayBuffer(pBuffer, DSBPLAY_LOOPING);
    }

    if(SUCCEEDED(hr))
    {
        Sleep(2000);
        
        hr = ValidateResult(pBuffer->GetCurrentPosition(&dwPlay, &dwWrite));
    }

    if(SUCCEEDED(hr))
    {
        DPF_TEST("play after natural stop (2000 ms delayed):  play %lu write %lu duration %lu", dwPlay, dwWrite, dwDuration);
    }

    if(SUCCEEDED(hr))
    {
        hr = PlayBuffer(pBuffer, DSBPLAY_FROMSTART);
    }

    if(SUCCEEDED(hr))
    {
        Sleep(1);
        
        hr = ValidateResult(pBuffer->GetCurrentPosition(&dwPlay, &dwWrite));
    }

    if(SUCCEEDED(hr))
    {
        DPF_TEST("play from start:  play %lu write %lu duration %lu", dwPlay, dwWrite, dwDuration);
    }

    if(SUCCEEDED(hr))
    {
        hr = WaitBuffer(pBuffer, FALSE);
    }

    RELEASE(pBuffer);

    return hr;
}


/****************************************************************************
 *
 *  TestFrequency
 *
 *  Description:
 *      Tests changing the frequency.
 *
 *  Arguments:
 *      DWORD [in]: buffer creation flags.
 *      DWORD [in]: format tag.
 *      DWORD [in]: sampling rate.
 *      DWORD [in]: bits per sample.
 *      DWORD [in]: channel count.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestFrequency"

HRESULT
TestFrequency
(
    DWORD                   dwFlags,
    DWORD                   dwFormatTag,
    DWORD                   dwSampleRate,
    DWORD                   dwBitsPerSample,
    DWORD                   dwChannelCount,
    DWORD                   dwDelay
)
{
    static const DWORD      adwFrequencies[]    = { 0, 8000, 11025, 16000, 22050, 32000, 44100, 48000, 64000, 96000 };
    LPDIRECTSOUNDBUFFER     pBuffer             = NULL;
    DWORD                   dwFrequency;
    HRESULT                 hr;

    hr = CreateStdBuffer(dwFlags, CreateFormat(dwFormatTag, dwSampleRate, dwBitsPerSample, dwChannelCount), 0, NULL, &pBuffer);

    if(SUCCEEDED(hr))
    {
        PlayBuffer(pBuffer, DSBPLAY_LOOPING);
    }

    if(SUCCEEDED(hr))
    {
        for(dwFrequency = 0; (dwFrequency < NUMELMS(adwFrequencies)) && SUCCEEDED(hr); dwFrequency++)
        {
            DPF_TEST("Frequency %lu", adwFrequencies[dwFrequency]);

            hr = ValidateResult(pBuffer->SetFrequency(adwFrequencies[dwFrequency]));

            if(SUCCEEDED(hr))
            {
                Sleep(dwDelay);
            }
        }
    }

    RELEASE(pBuffer);

    return hr;
}


/****************************************************************************
 *
 *  TestDiscontinuity
 *
 *  Description:
 *      Tests allowing the stream to starve by calling Discontinuity.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestDiscontinuity"

HRESULT
TestDiscontinuity
(
    DWORD                   dwFlags,
    DWORD                   dwFormatTag,
    DWORD                   dwSampleRate,
    DWORD                   dwBitsPerSample,
    DWORD                   dwChannelCount,
    DWORD                   dwPacketSize,
    BOOL                    fStarve
)
{
    LPCWAVEFORMATEX         pwfxFormat          = CreateFormat(dwFormatTag, dwSampleRate, dwBitsPerSample, dwChannelCount);
    LPDIRECTSOUNDSTREAM     pStream             = NULL;
    LPVOID                  pvDataBuffer        = NULL;
    DWORD                   dwPacketsThisTime   = 0;
    DWORD                   dwDuration;
    DWORD                   dwPacketCount;
    XMEDIAPACKET            xmb;
    HRESULT                 hr;
    DWORD                   i, z;

    //
    // Create the stream

    hr = CreateStdStream(dwFlags, pwfxFormat, 0, &dwPacketSize, NULL, NULL, NULL, &pStream, &pvDataBuffer, &dwDuration, &dwPacketCount);

    //
    // Enter the stream loop
    //

    if(SUCCEEDED(hr))
    {
        ZeroMemory(&xmb, sizeof(xmb));
        
        xmb.pvBuffer = pvDataBuffer;
        xmb.dwMaxSize = dwPacketSize;

        while(dwPacketCount)
        {
            dwPacketsThisTime = ((dwPacketsThisTime + 1) % 2) + 1;

            for(z = 0; (z < dwPacketsThisTime) && dwPacketCount && SUCCEEDED(hr); z++)
            {
                DirectSoundDoWork();
            
                if(!--dwPacketCount)
                {
                    xmb.dwMaxSize = dwDuration;
                }                
            
                hr = ValidateResult(pStream->Process(&xmb, NULL));

                xmb.pvBuffer = (LPBYTE)xmb.pvBuffer + xmb.dwMaxSize;
            
                dwDuration -= xmb.dwMaxSize;
            }

            if(SUCCEEDED(hr) && !fStarve)
            {
                hr = ValidateResult(pStream->Discontinuity());
            }

            if(SUCCEEDED(hr))
            {
                Sleep((dwPacketSize * dwPacketsThisTime * 2) / (pwfxFormat->nAvgBytesPerSec / 1000));
            }
        }
    }

    //
    // Clean up
    //

    RELEASE(pStream);
    FREE(pvDataBuffer);

    return hr;
}


/****************************************************************************
 *
 *  TestDeferredCommandRelease
 *
 *  Description:
 *      Tests whether deferred commands automatically get released when the
 *      buffer is.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestDeferredCommandRelease"

HRESULT
TestDeferredCommandRelease
(
    void
)
{
    LPDIRECTSOUNDBUFFER     pBuffer = NULL;
    HRESULT                 hr;

    hr = CreateStdBuffer(0, CreateFormat(WAVE_FORMAT_PCM, 32000, 16, 2), 0, NULL, &pBuffer);

    if(SUCCEEDED(hr))
    {
        hr = PlayBuffer(pBuffer, DSBPLAY_LOOPING);
    }

    if(SUCCEEDED(hr))
    {
        hr = StopBuffer(pBuffer, DSBSTOPEX_ENVELOPE, 10000);
    }

    RELEASE(pBuffer);

    return hr;
}


/****************************************************************************
 *
 *  TestHeadroom
 *
 *  Description:
 *      Tests headroom.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#if 0

#undef DPF_FNAME
#define DPF_FNAME "TestHeadroom"

HRESULT
TestHeadroom
(
    void
)
{
    static const DWORD      dwSleep         = 2000;
    LPDIRECTSOUNDBUFFER     pBuffer         = NULL;
    HRESULT                 hr;
    int                     i;

    hr = CreateToneBuffer(0, 8000, 16, 1, 500, 0, 0, &pBuffer);

    if(SUCCEEDED(hr))
    {
        hr = PlayBuffer(pBuffer, DSBPLAY_LOOPING);
    }

    for(i = DSBHEADROOM_MAX; SUCCEEDED(hr) && (i >= DSBHEADROOM_MIN); i -= 1000)
    {
        DPF_TEST("%lu voice headroom", i);

        hr = ValidateResult(pBuffer->SetHeadroom(i));

        if(SUCCEEDED(hr))
        {
            Sleep(dwSleep);
        }
    }

    for(i = DSHEADROOM_MIN; SUCCEEDED(hr) && (i <= DSHEADROOM_MAX); i++)
    {
        DPF_TEST("%lu mixbin headroom", i);

        hr = ValidateResult(g_pDirectSound->SetMixBinHeadroom(DSMIXBIN_VALID, i));

        if(SUCCEEDED(hr))
        {
            Sleep(dwSleep);
        }
    }

    RELEASE(pBuffer);

    return hr;
}

#endif


/****************************************************************************
 *
 *  TestI3DL2Listener
 *
 *  Description:
 *      Tests I3DL2 listener.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#if 0

#undef DPF_FNAME
#define DPF_FNAME "TestI3DL2Listener"

HRESULT
TestI3DL2Listener
(
    void
)
{
    static const struct
    {
        LPCSTR                      pszName;
        DSI3DL2LISTENER             Properties;
    } Environments[] =
    {
        { "Default",         { DSI3DL2_ENVIRONMENT_PRESET_DEFAULT } },
        { "Generic",         { DSI3DL2_ENVIRONMENT_PRESET_GENERIC } },
        { "PaddedCell",      { DSI3DL2_ENVIRONMENT_PRESET_PADDEDCELL } },
        { "Room",            { DSI3DL2_ENVIRONMENT_PRESET_ROOM } },
        { "Bathroom",        { DSI3DL2_ENVIRONMENT_PRESET_BATHROOM } },
        { "LivingRoom",      { DSI3DL2_ENVIRONMENT_PRESET_LIVINGROOM } },
        { "StoneRoom",       { DSI3DL2_ENVIRONMENT_PRESET_STONEROOM } },
        { "Auditorium",      { DSI3DL2_ENVIRONMENT_PRESET_AUDITORIUM } },
        { "ConcertHall",     { DSI3DL2_ENVIRONMENT_PRESET_CONCERTHALL } },
        { "Cave",            { DSI3DL2_ENVIRONMENT_PRESET_CAVE } },
        { "Arena",           { DSI3DL2_ENVIRONMENT_PRESET_ARENA } },
        { "Hangar",          { DSI3DL2_ENVIRONMENT_PRESET_HANGAR } },
        { "CarpetedHallway", { DSI3DL2_ENVIRONMENT_PRESET_CARPETEDHALLWAY } },
        { "Hallway",         { DSI3DL2_ENVIRONMENT_PRESET_HALLWAY } },
        { "StoneCorridor",   { DSI3DL2_ENVIRONMENT_PRESET_STONECORRIDOR } },
        { "Alley",           { DSI3DL2_ENVIRONMENT_PRESET_ALLEY } },
        { "Forest",          { DSI3DL2_ENVIRONMENT_PRESET_FOREST } },
        { "City",            { DSI3DL2_ENVIRONMENT_PRESET_CITY } },
        { "Mountains",       { DSI3DL2_ENVIRONMENT_PRESET_MOUNTAINS } },
        { "Quarry",          { DSI3DL2_ENVIRONMENT_PRESET_QUARRY } },
        { "Plain",           { DSI3DL2_ENVIRONMENT_PRESET_PLAIN } },
        { "ParkingLot",      { DSI3DL2_ENVIRONMENT_PRESET_PARKINGLOT } },
        { "SewerPipe",       { DSI3DL2_ENVIRONMENT_PRESET_SEWERPIPE } },
        { "UnderWater",      { DSI3DL2_ENVIRONMENT_PRESET_UNDERWATER } },
    };

    static const DWORD              pMixBins    = DSMIXBIN_SPEAKER_MASK | DSMIXBIN_I3DL2;
    static const DWORD              dwDelay         = 10000;
    LPDIRECTSOUNDBUFFER             pBuffer         = NULL;
    HRESULT                         hr;
    DWORD                           i;

    hr = CreateBufferFromFile("t:\\media\\audio\\pcm\\CleanGuitarArpeggios.wav", 0, pMixBins, NULL, &pBuffer);

    for(i = 0; SUCCEEDED(hr) && (i < NUMELMS(Environments)); i++)
    {
        DPF_TEST(Environments[i].pszName);

        hr = ValidateResult(g_pDirectSound->SetI3DL2Listener(&Environments[i].Properties, DS3D_IMMEDIATE));

        if(SUCCEEDED(hr))
        {
            hr = PlayBuffer(pBuffer, DSBPLAY_LOOPING);
        }

        if(SUCCEEDED(hr))
        {
            Sleep(dwDelay);
        }
    }

    RELEASE(pBuffer);

    return hr;
}

#endif


/****************************************************************************
 *
 *  TestMultiChannel
 *
 *  Description:
 *      Tests multichannel content.
 *
 *  Arguments:
 *      DWORD [in]: sampling rate.
 *      DWORD [in]: bits per sample.
 *      DWORD [in]: channel count.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestMultiChannel"

HRESULT
TestMultiChannel
(
    DWORD                   dwSampleRate,
    DWORD                   dwBitsPerSample,
    DWORD                   dwChannelCount
)
{
    LPCWAVEFORMATEX         pwfxSrc             = CreateFormat(WAVE_FORMAT_PCM, dwSampleRate, dwBitsPerSample, dwChannelCount);
    XFileMediaObject *      pWaveFile           = NULL;
    LPVOID                  pvSrcData           = NULL;
    LPVOID                  pvDstData           = NULL;
    LPDIRECTSOUNDBUFFER     pBuffer             = NULL;
    WAVEFORMATEX            wfxDst;
    DWORD                   dwSrcDataSize;
    DWORD                   dwDstDataSize;
    DSBUFFERDESC            dsbd;
    HRESULT                 hr;

    ZeroMemory(&dsbd, sizeof(dsbd));

    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = 0;
    dsbd.lpwfxFormat = &wfxDst;
    
    //
    // Load the source wave file
    //

    hr = LoadStdWaveFile(pwfxSrc, &pvSrcData, &dwSrcDataSize, &pWaveFile);

    //
    // Enter the multichannel test loop
    //

    if(SUCCEEDED(hr))
    {
        for(wfxDst.nChannels = pwfxSrc->nChannels + 1; wfxDst.nChannels <= 6; wfxDst.nChannels++)
        {
            if(wfxDst.nChannels % pwfxSrc->nChannels)
            {
                continue;
            }

            if(wfxDst.nChannels % 2)
            {
                continue;
            }

            //
            // Convert to multichannel
            //

            DPF_TEST("Converting from %lu channel to %lu channel", pwfxSrc->nChannels, wfxDst.nChannels);
            
            hr = MakeMultiChannel(pwfxSrc, &wfxDst, pvSrcData, dwSrcDataSize, &pvDstData, &dwDstDataSize);

            //
            // Create the buffer, making sure to keep it to 2047 SGEs
            //
            
            if(SUCCEEDED(hr))
            {
                hr = ValidateResult(DirectSoundCreateBuffer(&dsbd, &pBuffer));
            }

            if(SUCCEEDED(hr))
            {
                hr = ClampBufferSize(&dwDstDataSize);
            }

            if(SUCCEEDED(hr))
            {
                hr = ValidateResult(pBuffer->SetBufferData(pvDstData, dwDstDataSize));
            }

            //
            // Start playback
            //

            if(SUCCEEDED(hr))
            {
                hr = PlayBufferToCompletion(pBuffer);
            }

            //
            // Clean up
            //

            FREE(pvDstData);
            RELEASE(pBuffer);
        }
    }

    //
    // Clean up
    //

    RELEASE(pWaveFile);
    FREE(pvSrcData);

    return hr;
}


/****************************************************************************
 *
 *  TestMultiChannel2
 *
 *  Description:
 *      Tests multichannel content.
 *
 *  Arguments:
 *      DWORD [in]: sampling rate.
 *      DWORD [in]: bits per sample.
 *      DWORD [in]: channel count.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestMultiChannel2"

HRESULT
TestMultiChannel2
(
    DWORD                   dwChannelCount
)
{
    static const DWORD      dwSegmentLength = 2;
    LPVOID                  pvMonoData      = NULL;
    LPVOID                  pvMultiData     = NULL;
    LPDIRECTSOUNDBUFFER     pBuffer         = NULL;
    WAVEFORMATEX            wfxMono;
    WAVEFORMATEX            wfxMulti;
    short *                 pnMonoData;
    short *                 pnMultiData;
    DSBUFFERDESC            dsbd;
    HRESULT                 hr;
    DWORD                   a, b, c;

    XAudioCreatePcmFormat(1, 48000, 16, &wfxMono);
    XAudioCreatePcmFormat(6, wfxMono.nSamplesPerSec, wfxMono.wBitsPerSample, &wfxMulti);

    hr = MALLOC(pvMonoData, BYTE, wfxMono.nAvgBytesPerSec * dwSegmentLength);

    if(SUCCEEDED(hr))
    {
        hr = MALLOC(pvMultiData, BYTE, wfxMulti.nAvgBytesPerSec * wfxMulti.nChannels * dwSegmentLength);
    }

    if(SUCCEEDED(hr))
    {
        GenerateTone(pvMonoData, wfxMono.nAvgBytesPerSec * dwSegmentLength, wfxMono.nSamplesPerSec, wfxMono.wBitsPerSample, wfxMono.nChannels, 500);
    }

    if(SUCCEEDED(hr))
    {
        pnMultiData = (short *)pvMultiData;
        
        for(a = 0; a < wfxMulti.nChannels; a++)
        {
            pnMonoData = (short *)pvMonoData;
            
            for(b = 0; b < wfxMulti.nSamplesPerSec * dwSegmentLength; b++)
            {
                for(c = 0; c < a; c++)
                {
                    *pnMultiData++ = 0;
                }

                *pnMultiData++ = *pnMonoData++;

                for(c = a + 1; c < wfxMulti.nChannels; c++)
                {
                    *pnMultiData++ = 0;
                }
            }
        }
    }

    if(SUCCEEDED(hr))
    {
        ZeroMemory(&dsbd, sizeof(dsbd));

        dsbd.dwSize = sizeof(dsbd);
        dsbd.lpwfxFormat = &wfxMulti;

        hr = ValidateResult(DirectSoundCreateBuffer(&dsbd, &pBuffer));
    }

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(pBuffer->SetBufferData(pvMultiData, wfxMulti.nAvgBytesPerSec * wfxMulti.nChannels * dwSegmentLength));
    }

    if(SUCCEEDED(hr))
    {
        hr = PlayBuffer(pBuffer);
    }

    if(SUCCEEDED(hr))
    {
        hr = WaitBuffer(pBuffer, FALSE);
    }

    RELEASE(pBuffer);

    FREE(pvMultiData);
    FREE(pvMonoData);

    return hr;
}


/****************************************************************************
 *
 *  TestMultiChannel3
 *
 *  Description:
 *      Tests multichannel content.
 *
 *  Arguments:
 *      DWORD [in]: sampling rate.
 *      DWORD [in]: bits per sample.
 *      DWORD [in]: channel count.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestMultiChannel3"

HRESULT
TestMultiChannel3
(
    DWORD                   dwChannelCount
)
{
    static const DWORD      dwSegmentLength = 2;
    LPVOID                  pvMonoData      = NULL;
    LPVOID                  pvMultiData     = NULL;
    LPDIRECTSOUNDSTREAM     pStream         = NULL;
    WAVEFORMATEX            wfxMono;
    WAVEFORMATEX            wfxMulti;
    short *                 pnMonoData;
    short *                 pnMultiData;
    DSSTREAMDESC            dssd;
    XMEDIAPACKET            xmp;
    DWORD                   dwStatus;
    HRESULT                 hr;
    DWORD                   a, b, c;

    XAudioCreatePcmFormat(1, 48000, 16, &wfxMono);
    XAudioCreatePcmFormat(6, wfxMono.nSamplesPerSec, wfxMono.wBitsPerSample, &wfxMulti);

    hr = MALLOC(pvMonoData, BYTE, wfxMono.nAvgBytesPerSec * dwSegmentLength);

    if(SUCCEEDED(hr))
    {
        hr = MALLOC(pvMultiData, BYTE, wfxMulti.nAvgBytesPerSec * wfxMulti.nChannels * dwSegmentLength);
    }

    if(SUCCEEDED(hr))
    {
        GenerateTone(pvMonoData, wfxMono.nAvgBytesPerSec * dwSegmentLength, wfxMono.nSamplesPerSec, wfxMono.wBitsPerSample, wfxMono.nChannels, 500);
    }

    if(SUCCEEDED(hr))
    {
        pnMultiData = (short *)pvMultiData;
        
        for(a = 0; a < wfxMulti.nChannels; a++)
        {
            pnMonoData = (short *)pvMonoData;
            
            for(b = 0; b < wfxMulti.nSamplesPerSec * dwSegmentLength; b++)
            {
                for(c = 0; c < a; c++)
                {
                    *pnMultiData++ = 0;
                }

                *pnMultiData++ = *pnMonoData++;

                for(c = a + 1; c < wfxMulti.nChannels; c++)
                {
                    *pnMultiData++ = 0;
                }
            }
        }
    }

    if(SUCCEEDED(hr))
    {
        ZeroMemory(&dssd, sizeof(dssd));

        dssd.lpwfxFormat = &wfxMulti;
        dssd.dwMaxAttachedPackets = 1;

        hr = ValidateResult(DirectSoundCreateStream(&dssd, &pStream));
    }

    if(SUCCEEDED(hr))
    {
        ZeroMemory(&xmp, sizeof(xmp));

        xmp.pvBuffer = pvMultiData;
        xmp.dwMaxSize = wfxMulti.nAvgBytesPerSec * wfxMulti.nChannels * dwSegmentLength;
        xmp.pdwStatus = &dwStatus;

        hr = ValidateResult(pStream->Process(&xmp, NULL));
    }

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(pStream->Discontinuity());
    }

    if(SUCCEEDED(hr))
    {
        while(XMEDIAPACKET_STATUS_PENDING == *((volatile DWORD *)&dwStatus))
        {
            DirectSoundDoWork();
        }
    }

    RELEASE(pStream);

    FREE(pvMultiData);
    FREE(pvMonoData);

    return hr;
}


/****************************************************************************
 *
 *  TestReleaseEnvelope
 *
 *  Description:
 *      Tests the hardware release envelope.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "TestReleaseEnvelope"

HRESULT
TestReleaseEnvelope
(
    void
)
{
    LPDIRECTSOUNDBUFFER     pBuffer = NULL;
    DSENVELOPEDESC          dsed;
    HRESULT                 hr;

    hr = ValidateResult(CreateToneBuffer(0, 8000, 16, 1, 500, 0, 0, &pBuffer));

    if(SUCCEEDED(hr))
    {
        ZeroMemory(&dsed, sizeof(dsed));

        dsed.dwEG = DSEG_MULTI;
        dsed.dwMode = DSEG_MODE_DISABLE;
        dsed.dwRelease = 1000;
        
        hr = ValidateResult(pBuffer->SetEG(&dsed));
    }

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(PlayBuffer(pBuffer, DSBPLAY_LOOPING));
    }

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(StopBuffer(pBuffer, DSBSTOPEX_ENVELOPE, 0, FALSE, TRUE));
    }

    RELEASE(pBuffer);

    return hr;
}


/****************************************************************************
 *
 *  CompareVolume
 *
 *  Description:
 *      Compares comparable 2D and 3D buffer volume.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#if 0

#undef DPF_FNAME
#define DPF_FNAME "CompareVolume"

HRESULT
CompareVolume
(
    FLOAT                   flDistance
)
{
    static const LONG       lExtra  = -600;
    const LONG              lVolume = (LONG)(-2000.0f * log10f(flDistance)) + lExtra;
    LPDIRECTSOUNDBUFFER     pBuffer = NULL;
    BOOL                    f3d     = FALSE;
    HRESULT                 hr      = DS_OK;

    while(SUCCEEDED(hr) && !fBreakTest)
    {
        DPF_TEST(f3d ? "3D" : "2D");
        
        hr = ValidateResult(CreateToneBuffer(f3d ? DSBCAPS_CTRL3D : 0, 8000, 16, 1, 500, 0, f3d ? 0 : DSMIXBIN_FRONT_RIGHT | DSMIXBIN_FRONT_LEFT, &pBuffer));

        if(SUCCEEDED(hr))
        {
            hr = ValidateResult(pBuffer->SetHeadroom(0));
        }

        if(SUCCEEDED(hr))
        {
            if(f3d)
            {
                hr = ValidateResult(pBuffer->SetPosition(0, 0, flDistance, DS3D_IMMEDIATE));
            }
            else
            {
                hr = ValidateResult(pBuffer->SetVolume(lVolume));
            }
        }

        if(SUCCEEDED(hr))
        {
            hr = ValidateResult(PlayBuffer(pBuffer, DSBPLAY_LOOPING));
        }

        if(SUCCEEDED(hr))
        {
            hr = ValidateResult(StopBuffer(pBuffer, 0, 2000, FALSE, TRUE));
        }

        RELEASE(pBuffer);

        f3d = !f3d;
    }

    RELEASE(pBuffer);

    return hr;
}

#endif


/****************************************************************************
 *
 *  PlayToneBuffer
 *
 *  Description:
 *      Plays a simple sine-wave buffer.
 *
 *  Arguments:
 *      DWORD [in]: buffer creation flags.
 *      DWORD [in]: sampling rate.
 *      DWORD [in]: bit resolution.
 *      DWORD [in]: channel count.
 *      DWORD [in]: tone frequency.
 *      DWORD [in]: length of time to play the buffer, in ms.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "PlayToneBuffer"

HRESULT
PlayToneBuffer
(
    DWORD                   dwFlags,
    DWORD                   dwSamplesPerSec,
    DWORD                   dwBitsPerSample,
    DWORD                   dwChannels,
    DWORD                   dwToneFrequency,
    DWORD                   dwTime
)
{
    LPDIRECTSOUNDBUFFER     pBuffer = NULL;
    HRESULT                 hr;

    hr = ValidateResult(CreateToneBuffer(dwFlags, dwSamplesPerSec, dwBitsPerSample, dwChannels, dwToneFrequency, 0, 0, &pBuffer));

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(PlayBuffer(pBuffer, DSBPLAY_LOOPING));
    }

    if(SUCCEEDED(hr) && (INFINITE != dwTime))
    {
        hr = ValidateResult(StopBuffer(pBuffer, 0, dwTime));
    }

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(WaitBuffer(pBuffer, FALSE));
    }

    RELEASE(pBuffer);

    return hr;
}


/****************************************************************************
 *
 *  TestNaturalStop
 *
 *  Description:
 *      Tests playing a buffer to completion.
 *
 *  Arguments:
 *      DWORD [in]: iterations.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestNaturalStop"

HRESULT
TestNaturalStop
(
    DWORD                   dwIterations
)
{
    LPDIRECTSOUNDBUFFER     pBuffer = NULL;
    HRESULT                 hr;

    hr = ValidateResult(CreateToneBuffer(0, 8000, 16, 1, 400, 400, 0, &pBuffer));

    while(SUCCEEDED(hr))
    {
        if(INFINITE != dwIterations)
        {
            if(!dwIterations--)
            {
                break;
            }
        }
    
        hr = ValidateResult(PlayBuffer(pBuffer));

        if(SUCCEEDED(hr))
        {
            hr = ValidateResult(WaitBuffer(pBuffer, FALSE));
        }
    }

    RELEASE(pBuffer);

    return hr;
}


/****************************************************************************
 *
 *  TestFlush
 *
 *  Description:
 *      Tests flushing a stream.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "TestFlush"

HRESULT
TestFlush
(
    void
)
{
    const LPCWAVEFORMATEX   pwfxFormat          = CreateFormat(WAVE_FORMAT_PCM, 44100, 16, 2);
    LPDIRECTSOUNDSTREAM     pStream             = NULL;
    DWORD                   dwPacketSize        = 0x1000;
    LPVOID                  pvAudioData         = NULL;
    DWORD                   dwAudioDataSize     = NULL;
    DWORD                   dwPacketCount       = 0;
    LPDWORD                 padwStatus          = NULL;
    XMEDIAPACKET            xmp;
    HRESULT                 hr;
    DWORD                   i;

    hr = ValidateResult(CreateStdStream(0, pwfxFormat, 0, &dwPacketSize, NULL, NULL, NULL, &pStream, &pvAudioData, &dwAudioDataSize, &dwPacketCount));

    if(SUCCEEDED(hr))
    {
        hr = MALLOC(padwStatus, DWORD, dwPacketCount);
    }

    if(SUCCEEDED(hr))
    {
        ZeroMemory(&xmp, sizeof(xmp));

        xmp.pvBuffer = pvAudioData;
        xmp.dwMaxSize = dwPacketSize;
        xmp.pdwStatus = padwStatus;
        
        for(i = 0; SUCCEEDED(hr) && (i < dwPacketCount); i++)
        {
            if(i + 1 == dwPacketCount)
            {
                xmp.dwMaxSize = dwAudioDataSize - (dwPacketSize * (dwPacketCount - 1));
            }
            
            hr = ValidateResult(pStream->Process(&xmp, NULL));

            if(SUCCEEDED(hr))
            {
                xmp.pvBuffer = (LPBYTE)xmp.pvBuffer + xmp.dwMaxSize;
                xmp.pdwStatus++;
            }
        }
    }

    if(SUCCEEDED(hr))
    {
        Sleep(5000);

        hr = ValidateResult(pStream->Flush());
    }

    if(SUCCEEDED(hr))
    {
        for(i = 0; i < dwPacketCount; i++)
        {
            if((XMEDIAPACKET_STATUS_SUCCESS != padwStatus[i]) && (XMEDIAPACKET_STATUS_FLUSHED != padwStatus[i]))
            {
                DPF_TEST("Packet %lu status %x", i, padwStatus[i]);
                hr = E_FAIL;
            }
        }
    }

    RELEASE(pStream);
    FREE(padwStatus);
    FREE(pvAudioData);

    return hr;
}


/****************************************************************************
 *
 *  TestPitch
 *
 *  Description:
 *      Tests pitch.
 *
 *  Arguments:
 *      DWORD [in]: buffer creation flags.
 *      DWORD [in]: format tag.
 *      DWORD [in]: sampling rate.
 *      DWORD [in]: bit resolution.
 *      DWORD [in]: channel count.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "TestPitch"

HRESULT
TestPitch
(
    DWORD                   dwFlags,
    DWORD                   dwFormatTag,
    DWORD                   dwSamplesPerSec,
    DWORD                   dwBitsPerSample,
    DWORD                   dwChannels
)
{
    static const LONG       alPitch[]   = { DSBPITCH_MIN, 0, DSBPITCH_MAX };
    const LPCWAVEFORMATEX   pwfxFormat  = CreateFormat(dwFormatTag, dwSamplesPerSec, dwBitsPerSample, dwChannels);
    LPDIRECTSOUNDBUFFER     pBuffer     = NULL;
    HRESULT                 hr;
    DWORD                   i;

    hr = ValidateResult(CreateStdBuffer(dwFlags, pwfxFormat, 0, NULL, &pBuffer, NULL));

    if(SUCCEEDED(hr))
    {
        for(i = 0; (i < NUMELMS(alPitch)) && SUCCEEDED(hr); i++)
        {
            DPF_TEST("Pitch %d", alPitch[i]);
            
            hr = ValidateResult(pBuffer->SetPitch(alPitch[i]));

            if(SUCCEEDED(hr))
            {
                hr = ValidateResult(PlayBuffer(pBuffer, DSBPLAY_LOOPING));
            }

            if(SUCCEEDED(hr))
            {
                Sleep(2000);
            }
        }
    }

    RELEASE(pBuffer);

    return hr;
}


/****************************************************************************
 *
 *  StressPlayStop
 *
 *  Description:
 *      Stresses calling Play and Stop.
 *
 *  Arguments:
 *      DWORD [in]: time to test, in ms.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "StressPlayStop"

HRESULT
StressPlayStop
(
    DWORD                       dwFlags,
    DWORD                       dwSourceCount,
    DWORD                       dwNoteOffMin,
    DWORD                       dwNoteOffMax,
    DWORD                       dwTestLength,
    DWORD                       dwStreamCount,
    DWORD                       dwStreamFrequency
)
{
    static const DWORD          DSBSTATUS_PENDINGSTOP   = 0x80000000;
    static const WORD           awSilence[32]           = { 0 };
    const WAVEFORMATEX          wfxSilence              = INIT_PCM_WAVEFORMAT(1, dwStreamFrequency, 16);
    LPDIRECTSOUNDBUFFER         apSourceBuffers[256]    = { NULL };
    DWORD                       adwStatus[256]          = { 0 };
    LPDIRECTSOUNDSTREAM         apStreams[256]          = { NULL };
    LPDIRECTSOUNDBUFFER         pDestBuffer             = NULL;
    DWORD                       dwPlayCount             = 0;
    DWORD                       dwStopCount             = 0;
    DWORD                       dwNoteOffCount          = 0;
    DWORD                       dwNaturalStopCount      = 0;
    DWORD                       dwSubmixCount           = 0;
    DWORD                       dwLoopCount             = 0;
    HRESULT                     hr                      = S_OK;
    DWORD                       dwStartTime;
    DWORD                       dwCurrentTime;
    DWORD                       dwStatus;
    DSCAPS                      dsc;
    DSBUFFERDESC                dsbd;
    DSSTREAMDESC                dssd;
    DSENVELOPEDESC              dseg;
    XMEDIAPACKET                xmp;
    DWORD                       i, z;

    //
    // Set up the envelope description for NoteOff
    //
    
    dseg.dwEG = DSEG_AMPLITUDE;
    dseg.dwMode = DSEG_MODE_HOLD;
    dseg.dwDelay = DSEG_DELAY_DEFAULT;
    dseg.dwAttack = DSEG_ATTACK_DEFAULT;
    dseg.dwHold = DSEG_HOLD_DEFAULT;
    dseg.dwDecay = DSEG_DECAY_DEFAULT;
    dseg.dwRelease = DSEG_RELEASE_DEFAULT;
    dseg.dwSustain = DSEG_SUSTAIN_DEFAULT;
    dseg.lPitchScale = DSEG_PITCHSCALE_DEFAULT;
    dseg.lFilterCutOff = DSEG_FILTERCUTOFF_DEFAULT;

    //
    // Create the submix buffer
    //

    ZeroMemory(&dsbd, sizeof(dsbd));

    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = DSBCAPS_MIXIN;

    hr = ValidateResult(DirectSoundCreateBuffer(&dsbd, &pDestBuffer));

    //
    // Create interference streams
    //

    if(SUCCEEDED(hr) && dwStreamCount)
    {
        DPF_TEST("Creating %lu interference streams", dwStreamCount);

        ZeroMemory(&dssd, sizeof(dssd));

        dssd.lpwfxFormat = (LPWAVEFORMATEX)&wfxSilence;
        dssd.dwMaxAttachedPackets = 2;
        dssd.lpfnCallback = ResubmitPacketCallback;
        
        ZeroMemory(&xmp, sizeof(xmp));

        xmp.pvBuffer = (LPVOID)awSilence;
        xmp.dwMaxSize = sizeof(awSilence);
        xmp.pContext = &xmp;

        for(i = 0; (i < dwStreamCount) && SUCCEEDED(hr); i++)
        {
            dssd.lpvContext = &apStreams[i];

            hr = ValidateResult(DirectSoundCreateStream(&dssd, &apStreams[i]));

            if(SUCCEEDED(hr))
            {
                hr = PauseStream(apStreams[i], TRUE);
            }

            for(z = 0; (z < dssd.dwMaxAttachedPackets) && SUCCEEDED(hr); z++)
            {
                hr = ValidateResult(apStreams[i]->Process(&xmp, NULL));
            }

            if(SUCCEEDED(hr))
            {
                hr = PauseStream(apStreams[i], FALSE);
            }
        }
    }

    //
    // Create source buffers
    //
    
    if(SUCCEEDED(hr))
    {
        hr = GetDirectSoundCaps(&dsc);
    }

    if(SUCCEEDED(hr))
    {
        dwSourceCount = min(dwSourceCount, dsc.dwFree2DBuffers + dsc.dwFree3DBuffers);

        DPF_TEST("Creating %lu buffers", dwSourceCount);

        for(i = 0; (i < dwSourceCount) && SUCCEEDED(hr); i++)
        {
            hr = ValidateResult(CreateToneBuffer(dwFlags, 32000, 16, 1, 500, 0, 0, &apSourceBuffers[i]));
        }
    }

    //
    // Save the current time
    //

    if(SUCCEEDED(hr))
    {
        dwStartTime = GetTickCount();
    }

    //
    // Enter the main loop
    //

    while(SUCCEEDED(hr))
    {
        DirectSoundDoWork();

        //
        // Check the current time
        //
        
        if(!((dwCurrentTime = GetTickCount()) % 2000))
        {
            DPF_TEST("%lu plays (%lu submixing, %lu looping), %lu natural stops, %lu forced, %lu note-off", dwPlayCount, dwSubmixCount, dwLoopCount, dwNaturalStopCount, dwStopCount, dwNoteOffCount);

            dwPlayCount = 0;
            dwStopCount = 0;
            dwNoteOffCount = 0;
            dwNaturalStopCount = 0;
            dwSubmixCount = 0;
            dwLoopCount = 0;

            if(INFINITE != dwTestLength)
            {
                if(dwCurrentTime >= dwStartTime + dwTestLength)
                {
                    break;
                }
            }
        }

        //
        // Pick a random buffer and get it's status
        //

        i = rand() % dwSourceCount;

        hr = ValidateResult(apSourceBuffers[i]->GetStatus(&dwStatus));

        if(SUCCEEDED(hr))
        {
            if(adwStatus[i] & DSBSTATUS_PENDINGSTOP)
            {
                if(!(dwStatus & DSBSTATUS_PLAYING))
                {
                    adwStatus[i] = 0;
                }
            }
            else
            {
                adwStatus[i] = dwStatus;
            }
        }

        //
        // Should we play or stop?
        //

        if(SUCCEEDED(hr) && !(adwStatus[i] & DSBSTATUS_PENDINGSTOP))
        {
            if(rand() % 2)
            {
                dwPlayCount++;
            
                //
                // Submix or not?
                //
            
                if(!(adwStatus[i] & DSBSTATUS_PLAYING))
                {
                    if(rand() % 2)
                    {
                        dwSubmixCount++;
                
                        hr = ValidateResult(apSourceBuffers[i]->SetOutputBuffer(pDestBuffer));
                    }
                    else
                    {
                        hr = ValidateResult(apSourceBuffers[i]->SetOutputBuffer(NULL));
                    }
                }

                //
                // Loop or not?
                //

                if(SUCCEEDED(hr)) 
                {
                    if(rand() % 2)
                    {
                        dwLoopCount++;
                
                        hr = ValidateResult(apSourceBuffers[i]->Play(0, 0, DSBPLAY_LOOPING));
                    }
                    else
                    {
                        hr = ValidateResult(apSourceBuffers[i]->Play(0, 0, 0));
                    }
                }
            }
            else
            {
                //
                // Still playing?
                //
            
                if(adwStatus[i] & DSBSTATUS_PLAYING)
                {
                    //
                    // Envelope or not?
                    //

                    if(rand() % 2)
                    {
                        dwNoteOffCount++;
                        
                        dseg.dwRelease = dwNoteOffMin + (rand() % (dwNoteOffMax + 1 - dwNoteOffMin));

                        hr = ValidateResult(apSourceBuffers[i]->SetEG(&dseg));

                        if(SUCCEEDED(hr))
                        {
                            hr = ValidateResult(apSourceBuffers[i]->StopEx(0, DSBSTOPEX_ENVELOPE));
                        }
                    }
                    else
                    {
                        dwStopCount++;

                        hr = ValidateResult(apSourceBuffers[i]->Stop());
                    }

                    if(SUCCEEDED(hr))
                    {
                        adwStatus[i] |= DSBSTATUS_PENDINGSTOP;
                    }
                }
                else
                {
                    dwNaturalStopCount++;
                }
            }
        }
    }

    for(i = 0; i < dwSourceCount; i++)
    {
        RELEASE(apSourceBuffers[i]);
    }

    for(i = 0; i < dwStreamCount; i++)
    {
        RELEASE(apStreams[i]);
    }

    RELEASE(pDestBuffer);

    return hr;
}    


/****************************************************************************
 *
 *  FindFrequencyRange
 *
 *  Description:
 *      Insert description text here.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "FindFrequencyRange"

HRESULT
FindFrequencyRange
(
    void
)
{
    LONG                    lPitch;
    DWORD                   i;
    
    for(i = 48000; i > 0; i--)
    {
        if((lPitch = XAudioCalculatePitch(i)) < DSBPITCH_MIN)
        {
            DPF_TEST("Lower %lu (pitch %ld)", i + 1, lPitch);
            break;
        }
    }

    for(i = 48000; ; i++)
    {
        if((lPitch = XAudioCalculatePitch(i)) > DSBPITCH_MAX)
        {
            DPF_TEST("Upper %lu (pitch %ld)", i - 1, lPitch);
            break;
        }
    }

    return DS_OK;
}


/****************************************************************************
 *
 *  TestDeltaPanic
 *
 *  Description:
 *      Main test function.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:
 *      (void)
 *
 ****************************************************************************/

#if 0

#undef DPF_FNAME
#define DPF_FNAME "TestDeltaPanic"

HRESULT
TestDeltaPanic
(
    void
)
{
    LPDIRECTSOUNDBUFFER     pToneBuffer     = NULL;
    HRESULT                 hr;

    hr = CreateToneBuffer(0, 8000, 8, 1, 500, 0, 0, &pToneBuffer);

    if(SUCCEEDED(hr))
    {
        hr = pToneBuffer->SetVolume(-2400);
    }

    if(SUCCEEDED(hr))
    {
        hr = PlayBuffer(pToneBuffer, DSBPLAY_LOOPING);
    }
    
    while(SUCCEEDED(hr))
    {

#ifdef DEBUG

        g_fDirectSoundSimulateDeltaPanic = TRUE;

#endif // DEBUG

        hr = PlayOneShotBuffer(0, WAVE_FORMAT_XBOX_ADPCM, 48000, 16, 1, DSMIXBIN_FXSEND_0);
    }

    RELEASE(pToneBuffer);

    return hr;
}

#endif


/****************************************************************************
 *
 *  TestTimeStampedPackets
 *
 *  Description:
 *      Tests stream packets with a timestamp.
 *
 *  Arguments:
 *      DWORD [in]: delay between packets, in milliseconds.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "TestTimeStampedPackets"

HRESULT
TestTimeStampedPackets
(
    DWORD                   dwDelay
)
{
    static const DWORD      dwPacketCount                   = 2;
    const LPCWAVEFORMATEX   pwfxFormat                      = CreateFormat(WAVE_FORMAT_PCM, 8000, 16, 1);
    const DWORD             dwBufferSize                    = pwfxFormat->nAvgBytesPerSec * 4;
    LPVOID                  pvPacketData                    = NULL;
    LPDIRECTSOUNDSTREAM     pStream                         = NULL;
    DWORD                   adwStatus[dwPacketCount];
    REFERENCE_TIME          rtPacketLength;
    REFERENCE_TIME          artTimeStamps[dwPacketCount];
    DSSTREAMDESC            dssd;
    XMEDIAPACKET            xmp;
    HRESULT                 hr;
    DWORD                   i;

    ZeroMemory(&dssd, sizeof(dssd));

    dssd.lpwfxFormat = (LPWAVEFORMATEX)pwfxFormat;
    dssd.dwMaxAttachedPackets = dwPacketCount;

    hr = ValidateResult(DirectSoundCreateStream(&dssd, &pStream));

    if(SUCCEEDED(hr))
    {
        hr = MALLOC(pvPacketData, BYTE, dwBufferSize);
    }

    if(SUCCEEDED(hr))
    {
        GenerateTone(pvPacketData, dwBufferSize, pwfxFormat->nSamplesPerSec, pwfxFormat->wBitsPerSample, pwfxFormat->nChannels, 500);

        ZeroMemory(&xmp, sizeof(xmp));

        xmp.pvBuffer = pvPacketData;
        xmp.dwMaxSize = dwBufferSize;

        rtPacketLength = ((REFERENCE_TIME)dwBufferSize * 10000000i64) / (REFERENCE_TIME)pwfxFormat->nAvgBytesPerSec;
        rtPacketLength += (REFERENCE_TIME)dwDelay * 10000i64;
        
        for(i = 0; (i < dwPacketCount) && SUCCEEDED(hr); i++)
        {
            artTimeStamps[i] = -(rtPacketLength * i);

            xmp.pdwStatus = &adwStatus[i];
            xmp.prtTimestamp = &artTimeStamps[i];

            hr = ValidateResult(pStream->Process(&xmp, NULL));
        }
    }

    if(SUCCEEDED(hr))
    {
        for(i = 0; i < dwPacketCount; i++)
        {
            while(XMEDIAPACKET_STATUS_PENDING == adwStatus[i])
            {
                DirectSoundDoWork();
            }
        }
    }

    RELEASE(pStream);
    FREE(pvPacketData);

    return hr;
}


/****************************************************************************
 *
 *  TestSetFormat
 *
 *  Description:
 *      Sets format on a buffer.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "TestSetFormat"

HRESULT
TestSetFormat
(
    void
)
{
    static const WAVEFORMATEX           wfxFormat1      = INIT_PCM_WAVEFORMAT(2, 44100, 16);
    static const XBOXADPCMWAVEFORMAT    wfxFormat2      = INIT_XBOX_ADPCM_WAVEFORMAT(1, 22050);
    static const DWORD                  dwDelay         = 2500;
    XFileMediaObject *                  pSrcXmo2        = NULL;
    LPVOID                              pvDataBuffer2   = NULL;
    IDirectSoundBuffer *                pBuffer         = NULL;
    DWORD                               dwLength2;
    HRESULT                             hr;
    
    hr = CreateStdBuffer(0, &wfxFormat1, NULL, NULL, &pBuffer);
    
    if(SUCCEEDED(hr))
    {
        hr = PlayBuffer(pBuffer, DSBPLAY_LOOPING);
    }

    if(SUCCEEDED(hr))
    {
        hr = StopBuffer(pBuffer, 0, dwDelay, TRUE, TRUE);
    }

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(pBuffer->SetBufferData(NULL, 0));
    }
    
    if(SUCCEEDED(hr))
    {
        hr = LoadStdWaveFile(&wfxFormat2.wfx, &pvDataBuffer2, &dwLength2, &pSrcXmo2);
    }

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(pBuffer->SetFormat(&wfxFormat2.wfx));
    }

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(pBuffer->SetBufferData(pvDataBuffer2, dwLength2));
    }

    if(SUCCEEDED(hr))
    {
        hr = PlayBuffer(pBuffer, DSBPLAY_LOOPING);
    }

    if(SUCCEEDED(hr))
    {
        hr = StopBuffer(pBuffer, 0, dwDelay, TRUE, TRUE);
    }

    RELEASE(pBuffer);        
    RELEASE(pSrcXmo2);
    FREE(pvDataBuffer2);

    return hr;
}


/****************************************************************************
 *
 *  TestStarvation
 *
 *  Description:
 *      Tests starving a stream.
 *
 *  Arguments:
 *      DWORD [in]: stream creation flags.
 *      DWORD [in]: format tag.
 *      DWORD [in]: sampling rate.
 *      DWORD [in]: bit resolution.
 *      DWORD [in]: channel count.
 *      DWORD [in]: packet size, in bytes.
 *      DWORD [in]: 1-in-N chance the stream will starve between packets.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestStarvation"

HRESULT
TestStarvation
(
    DWORD                   dwFlags,
    DWORD                   dwFormatTag,
    DWORD                   dwSamplesPerSec,
    DWORD                   dwBitsPerSample,
    DWORD                   dwChannelCount,
    DWORD                   dwPacketSize,
    DWORD                   dwStarvationChance
)
{
    const LPCWAVEFORMATEX   pwfxFormat      = CreateFormat(dwFormatTag, dwSamplesPerSec, dwBitsPerSample, dwChannelCount);
    LPDIRECTSOUNDSTREAM     pStream         = NULL;
    LPVOID                  pvAudioData     = NULL;
    DWORD                   dwAudioDataSize = 0;
    DWORD                   dwPacketCount   = 0;
    LPDWORD                 padwStatus      = NULL;
    BOOL                    fStarve;
    XMEDIAPACKET            xmp;
    HRESULT                 hr;

    hr = CreateStdStream(dwFlags, pwfxFormat, NULL, &dwPacketSize, NULL, NULL, NULL, &pStream, &pvAudioData, &dwAudioDataSize, &dwPacketCount);

    if(SUCCEEDED(hr))
    {
        hr = MALLOC(padwStatus, DWORD, dwPacketCount);
    }

    if(SUCCEEDED(hr))
    {
        ZeroMemory(&xmp, sizeof(xmp));

        xmp.pvBuffer = pvAudioData;
        xmp.dwMaxSize = dwPacketSize;
        xmp.pdwStatus = padwStatus;
    }

    while(SUCCEEDED(hr) && dwPacketCount--)
    {
        if(!dwPacketCount)
        {
            xmp.dwMaxSize = dwAudioDataSize;
        }

        DPF_TEST("Feeding the stream");

        hr = ValidateResult(pStream->Process(&xmp, NULL));

        if(SUCCEEDED(hr))
        {
            fStarve = !(rand() % dwStarvationChance);
            
            if(!dwPacketCount || fStarve)
            {
                if(fStarve)
                {
                    DPF_TEST("Starving the stream");
                }

                while(XMEDIAPACKET_STATUS_PENDING == *xmp.pdwStatus)
                {
                    DirectSoundDoWork();
                }
            }
        }

        xmp.pvBuffer = (LPBYTE)xmp.pvBuffer + xmp.dwMaxSize;
        
        xmp.pdwStatus++;

        dwAudioDataSize -= xmp.dwMaxSize;
    }

    RELEASE(pStream);

    FREE(pvAudioData);
    FREE(padwStatus);

    return hr;
}


/****************************************************************************
 *
 *  TestWaveBundler
 *
 *  Description:
 *      Test the Wave Bundler tool.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestWaveBundler"

HRESULT
TestWaveBundler
(
    LPCSTR                  pszBank,
    DWORD                   dwEntryCount
)
{
    LPDIRECTSOUNDBUFFER     pBuffer     = NULL;
    CWaveBankReader         WaveBank;
    DSBUFFERDESC            dsbd;
    WAVEBANKSECTIONDATA     SectionData;
    WAVEBANKUNIWAVEFORMAT   wfx;
    WAVEBANKENTRYREGION     PlayRegion;
    WAVEBANKENTRYREGION     LoopRegion;
    DWORD                   dwSize;
    BOOL                    fMapped;
    HRESULT                 hr;
    DWORD                   i;

    DPF_ENTER();

    DPF_TEST("Opening wave bank %s", pszBank);
    
    hr = ValidateResult(WaveBank.Open(pszBank));

    if(SUCCEEDED(hr))
    {
        WaveBank.GetSectionData(&SectionData);
    }

    if(SUCCEEDED(hr))
    {  
        ASSERT(dwEntryCount == SectionData.pHeader->dwEntryCount);
    }

    if(SUCCEEDED(hr))
    {
        ZeroMemory(&dsbd, sizeof(dsbd));

        dsbd.dwSize = sizeof(dsbd);
        dsbd.lpwfxFormat = (LPWAVEFORMATEX)CreateFormat(WAVE_FORMAT_PCM, 44100, 16, 2);

        hr = ValidateResult(DirectSoundCreateBuffer(&dsbd, &pBuffer));
    }

    if(SUCCEEDED(hr))
    {
        dwSize = SectionData.dwDataSize;

        hr = ClampBufferSize(&dwSize);
    }

    if(SUCCEEDED(hr))
    {
        if(fMapped = (dwSize == SectionData.dwDataSize))
        {
            hr = ValidateResult(pBuffer->SetBufferData(SectionData.pvData, SectionData.dwDataSize));

            if(SUCCEEDED(hr))
            {
                DPF_TEST("Entire bank mapped.  Using SetPlayRegion");
            }
        }
        else
        {
            DPF_TEST("Bank too big to map.  Using SetBufferData for each element");
        }
    }

    for(i = 0; (i < dwEntryCount) && SUCCEEDED(hr); i++)
    {
        WaveBankExpandFormat(&SectionData.paMetaData[i].Format, &wfx);
        
        PlayRegion = SectionData.paMetaData[i].PlayRegion;
        LoopRegion = SectionData.paMetaData[i].LoopRegion;
        
        DPF_TEST("Loading entry %lu (%s, %lu.%3.3lu kHz, %lu-bit, %lu-channel, offset %lu, length %lu)", i, (WAVE_FORMAT_XBOX_ADPCM == wfx.WaveFormatEx.wFormatTag) ? "ADPCM" : "PCM", wfx.WaveFormatEx.nSamplesPerSec / 1000, wfx.WaveFormatEx.nSamplesPerSec % 1000, wfx.WaveFormatEx.wBitsPerSample, wfx.WaveFormatEx.nChannels, PlayRegion.dwStart, PlayRegion.dwLength);
        
        hr = ValidateResult(pBuffer->SetFormat(&wfx.WaveFormatEx));

        if(SUCCEEDED(hr))
        {
            if(fMapped)
            {
                hr = ValidateResult(pBuffer->SetPlayRegion(PlayRegion.dwStart, PlayRegion.dwLength));
            }
            else
            {
                ClampBufferSize(&PlayRegion.dwLength);
            
                hr = ValidateResult(pBuffer->SetBufferData((LPBYTE)SectionData.pvData + PlayRegion.dwStart, PlayRegion.dwLength));
            }
        }

        if(SUCCEEDED(hr))
        {
            hr = ValidateResult(pBuffer->SetLoopRegion(LoopRegion.dwStart, LoopRegion.dwLength));
        }

        if(SUCCEEDED(hr))
        {
            hr = PlayBuffer(pBuffer, LoopRegion.dwLength ? DSBPLAY_LOOPING : 0);
        }

        if(SUCCEEDED(hr) && LoopRegion.dwLength)
        {
            Sleep(PlayRegion.dwLength * 2 * 1000 / wfx.WaveFormatEx.nAvgBytesPerSec);

            hr = PlayBuffer(pBuffer);
        }

        if(SUCCEEDED(hr))
        {
            hr = WaitBuffer(pBuffer, FALSE);
        }
    }

    RELEASE(pBuffer);

    DPF_LEAVE_HRESULT(hr);
    
    return hr;
}


/****************************************************************************
 *
 *  TestReleasePausedStream
 *
 *  Description:
 *      Tests releasing a paused stream.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestReleasePausedStream"

HRESULT
TestReleasePausedStream
(
    void
)
{
    LPDIRECTSOUNDSTREAM     pStream = NULL;
    DSSTREAMDESC            dssd;
    HRESULT                 hr;
    
    DPF_ENTER();

    ZeroMemory(&dssd, sizeof(dssd));

    dssd.lpwfxFormat = (LPWAVEFORMATEX)CreateFormat(WAVE_FORMAT_PCM, 44100, 16, 2);
    dssd.dwMaxAttachedPackets = 1;

    hr = DirectSoundCreateStream(&dssd, &pStream);

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(pStream->Pause(DSSTREAMPAUSE_PAUSE));
    }

    RELEASE(pStream);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  TestTimedStreamResume
 *
 *  Description:
 *      Tests resuming a paused stream using a timestamp.
 *
 *  Arguments:
 *      DWORD [in]: delay, in ms.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestTimedStreamResume"

HRESULT
TestTimedStreamResume
(
    DWORD                   dwDelay
)
{
    const LPCWAVEFORMATEX   pwfxFormat          = CreateFormat(WAVE_FORMAT_PCM, 48000, 16, 2);
    const REFERENCE_TIME    rtTimestamp         = (REFERENCE_TIME)dwDelay * -10000i64;
    LPDIRECTSOUNDSTREAM     pStream             = NULL;
    LPVOID                  pvAudioData         = NULL;
    DWORD                   dwPacketSize        = 0x4000;
    HRESULT                 hr;

    hr = CreateStdQueuedStream(0, pwfxFormat, NULL, &dwPacketSize, NULL, NULL, &pStream, &pvAudioData);

    if(SUCCEEDED(hr))
    {
        DPF_TEST("Setting Pause command for %lu ms in the future", dwDelay);
        
        hr = ValidateResult(pStream->PauseEx(rtTimestamp, DSSTREAMPAUSE_RESUME));
    }

    if(SUCCEEDED(hr))
    {
        hr = WaitStream(pStream, TRUE);
    }

    if(SUCCEEDED(hr))
    {
        hr = WaitStream(pStream, FALSE);
    }

    RELEASE(pStream);
    FREE(pvAudioData);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  TestDirectSound
 *
 *  Description:
 *      Main test function.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestDirectSound"

HRESULT
TestDirectSound
(
    void
)
{
    HRESULT                 hr  = DS_OK;

    DirectSoundUsePan3D();

    //
    // Create the DirectSound object
    //

    hr = ValidateResult(DirectSoundCreate(NULL, &g_pDirectSound, NULL));

    //
    // Load the default effects image
    //

    if(SUCCEEDED(hr))
    {
        hr = LoadEffectsImage("dsstdfx", XAUDIO_DOWNLOADFX_XBESECTION);
    }
    
    //
    // Test away...
    //

    // BEGIN_TEST(PlayOneShotBuffer)(0, WAVE_FORMAT_PCM, 48000, 16, 1);
    // BEGIN_TEST(PlayOneShotBuffer)("t:\\media\\audio\\pcm\\48k16bs.aif");
    // BEGIN_TEST(PlayOneShotStream)(0, WAVE_FORMAT_XBOX_ADPCM, 48000, 16, 1, 0x4000);
    // BEGIN_TEST(PlayToneBuffer)(0, 8000, 8, 1, 400, INFINITE);
    // BEGIN_TEST(Test3d)(0.0f, 0.0f, 0.0f, 5.0f, -72.0f, INFINITE);
    // BEGIN_TEST(TestMultipleBuffers)(WAVE_FORMAT_PCM);
    // BEGIN_TEST(TestMultipleStreams)(WAVE_FORMAT_PCM, 0x4000);
    // BEGIN_TEST(TestSequentialBuffers)(WAVE_FORMAT_PCM);
    // BEGIN_TEST(TestNotifies)(WAVE_FORMAT_PCM, 22050, 16, 1, 20, FALSE);
    // BEGIN_TEST(TestBufferOffsets)(WAVE_FORMAT_PCM, 48000, 16, 2, 0.0f, 1.0f, 0.5f, 1.0f, TRUE);
    // BEGIN_TEST(TestSubMix)(TRUE, WAVE_FORMAT_PCM);
    // BEGIN_TEST(TestDLS)(WAVE_FORMAT_PCM, 44100, 16, 1);
    // BEGIN_TEST(TestSurroundEncoder)(WAVE_FORMAT_PCM, 44100, 16);
    // BEGIN_TEST(TestSgeSharing)();
    // BEGIN_TEST(TestSgeHeap)();
    // BEGIN_TEST(Test256Voices)();
    // BEGIN_TEST(TestStop)(WAVE_FORMAT_PCM, 44100, 16, 1);
    // BEGIN_TEST(TestFrequency)(DSBCAPS_CTRL3D, WAVE_FORMAT_PCM, 22050, 16, 1, 5000);
    // BEGIN_TEST(TestDiscontinuity)(0, WAVE_FORMAT_PCM, 22050, 16, 1, 0x4000, TRUE);
    // BEGIN_TEST(TestDeferredCommandRelease)();
    // BEGIN_TEST(TestHeadroom)();
    // BEGIN_TEST(TestI3DL2Listener)();
    // BEGIN_TEST(TestMultiChannel)(44100, 8, 1);
    // BEGIN_TEST(TestMultiChannel2)(6);
    // BEGIN_TEST(TestMultiChannel3)(6);
    // BEGIN_TEST(TestReverb)();
    // BEGIN_TEST(StressPlayStop)(0, -1, 1, 50, INFINITE, 0, 8000);
    // BEGIN_TEST(TestReleaseEnvelope)();
    // BEGIN_TEST(TestNaturalStop)(1);
    // BEGIN_TEST(TestFlush)();
    // BEGIN_TEST(TestPitch)(0, WAVE_FORMAT_PCM, 44100, 16, 1);
    // BEGIN_TEST(FindFrequencyRange)();
    // BEGIN_TEST(TestTimeStampedPackets)(5000);
    // BEGIN_TEST(TestDeltaPanic)();
    // BEGIN_TEST(TestSetFormat)();
    // BEGIN_TEST(TestStarvation)(0, WAVE_FORMAT_XBOX_ADPCM, 48000, 16, 1, 0x4000, 3);
    // BEGIN_TEST(TestStreamStop)(DSSTREAMFLUSHEX_IMMEDIATE, WAVE_FORMAT_PCM, 44100, 16, 2, 0x4000);
    // BEGIN_TEST(TestStreamStop)(DSSTREAMFLUSHEX_ASYNC, WAVE_FORMAT_PCM, 44100, 16, 2, 0x4000);
    // BEGIN_TEST(TestStreamStop)(DSSTREAMFLUSHEX_ASYNC | DSSTREAMFLUSHEX_ENVELOPE, WAVE_FORMAT_PCM, 44100, 16, 2, 0x4000);
    // BEGIN_TEST(TestReleasePausedStream)();
    BEGIN_TEST(TestTimedStreamResume)(10000);
    // BEGIN_TEST(TestWaveBundler)("t:\\media\\audio\\pcm.xwb", WAVEBANK_PCM_ENTRY_COUNT);
    // BEGIN_TEST(TestWaveBundler)("t:\\media\\audio\\pcmaiff.xwb", WAVEBANK_PCMAIFF_ENTRY_COUNT);
    // BEGIN_TEST(TestWaveBundler)("t:\\media\\audio\\adpcm.xwb", WAVEBANK_ADPCM_ENTRY_COUNT);
    // BEGIN_TEST(TestWaveBundler)("t:\\media\\audio\\compress.xwb", WAVEBANK_COMPRESS_ENTRY_COUNT);
    // BEGIN_TEST(TestWaveBundler)("t:\\media\\audio\\8bit.xwb", WAVEBANK_8BIT_ENTRY_COUNT);
    // BEGIN_TEST(TestWaveBundler)("t:\\media\\audio\\loop.xwb", WAVEBANK_LOOP_ENTRY_COUNT);
    // BEGIN_TEST(TestWaveBundler)("t:\\media\\audio\\mixed.xwb", WAVEBANK_MIXED_ENTRY_COUNT);

    //
    // Release the DirectSound object
    //

    RELEASE(g_pDirectSound);

    return hr;
}


