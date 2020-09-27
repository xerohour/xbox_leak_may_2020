/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       DsoundDRT.cpp
 *  Content:    DsoundDRT tests
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  04/20/01    danrose Created to test Xbox DsoundDRT
 *
 ****************************************************************************/

#include "DsoundDRT.h"
#include "medialoader.h"

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;
extern "C" g_fDirectSoundDisableBusyWaitWarning;

/****************************************************************************
 *
 *  TestAc97XmoCallback
 *
 *  Description:
 *      Callback function for TestAc97Xmo.
 *
 *  Arguments:
 *      LPVOID [in]: stream context.
 *      LPVOID [in]: packet context.
 *      DWORD [in]: packet status.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestAc97XmoCallback"

void CALLBACK
TestAc97XmoCallback
(
    LPVOID                  pvStreamContext,
    LPVOID                  pvPacketContext,
    DWORD                   dwStatus
)
{
    LPDWORD                 pdwSubmittedMask    = (LPDWORD)pvStreamContext;
    DWORD                   dwPacketIndex       = (DWORD)pvPacketContext;

    *pdwSubmittedMask &= ~(1UL << dwPacketIndex);
}    


/****************************************************************************
 *
 *  TestAc97Xmo
 *
 *  Description:
 *      Tests the AC97 XMO.
 *
 *  Arguments:
 *      DWORD [in]: channel index.
 *      DWORD [in]: channel mode.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestAc97XmoCallback"

HRESULT
TestAc97Xmo
(
    DWORD                   dwChannel,
    DWORD                   dwMode
)
{
    static const DWORD      dwPacketSize        = 0x1000;
    XAc97MediaObject *      pDevice             = NULL;
    XFileMediaObject *      pWaveFile           = NULL;
    LPVOID                  pvBufferData        = NULL;
    volatile DWORD          dwSubmittedMask     = 0;
    DWORD                   dwSubmittedSize     = 0;
    XMEDIAPACKET            xmp;
    DWORD                   dwRead;
    DWORD                   dwPosition;
    DWORD                   i;
    HRESULT                 hr;

    hr = ValidateResult(Ac97CreateMediaObject(dwChannel, TestAc97XmoCallback, (LPVOID)&dwSubmittedMask, &pDevice));

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(pDevice->SetMode(dwMode));
    }

    if(SUCCEEDED(hr))
    {
        if(DSAC97_MODE_ENCODED == dwMode)
        {
            hr = ValidateResult(LoadWaveFile("t:\\media\\audio\\ac3\\dd-broadway.wav", NULL, &pWaveFile));
        }
        else
        {
            hr = ValidateResult(LoadStdWaveFile(CreateFormat(WAVE_FORMAT_PCM, 48000, 16, 2), &pWaveFile));
        }
    }

    if(SUCCEEDED(hr))
    {
        hr = ValidatePtr(pvBufferData = XPhysicalAlloc(dwPacketSize * DSAC97_MAX_ATTACHED_PACKETS, ~0UL, 4096, PAGE_READWRITE));
    }

    while(SUCCEEDED(hr))
    {
        while(((1UL << DSAC97_MAX_ATTACHED_PACKETS) - 1) == dwSubmittedMask);

        for(i = 0; i < DSAC97_MAX_ATTACHED_PACKETS; i++)
        {
            if(!(dwSubmittedMask & (1UL << i)))
            {
                break;
            }
        }

        ASSERT(i < DSAC97_MAX_ATTACHED_PACKETS);

        xmp.pvBuffer = (LPBYTE)pvBufferData + (dwPacketSize * i);
        xmp.dwMaxSize = dwPacketSize;
        xmp.pdwCompletedSize = &dwRead;
        xmp.pdwStatus = NULL;
        xmp.pContext = NULL;
        xmp.prtTimestamp = NULL;

        hr = ValidateResult(pWaveFile->Process(NULL, &xmp));

        if(SUCCEEDED(hr))
        {
            if(dwRead)
            {
                dwSubmittedMask |= 1UL << i;

                xmp.dwMaxSize = dwRead;
                xmp.pdwCompletedSize = NULL;
                xmp.pContext = (LPVOID)i;

                dwSubmittedSize += dwRead;

                hr = ValidateResult(pDevice->Process(&xmp, NULL));

                if(SUCCEEDED(hr))
                {
                    hr = ValidateResult(pDevice->Discontinuity());
                }

                if(SUCCEEDED(hr))
                {
                    hr = pDevice->GetCurrentPosition(&dwPosition);
                }

                if(SUCCEEDED(hr))
                {
                    DbgPrint("submitted %lu, position %lu\n", dwSubmittedSize, dwPosition);
                }
            }
            else
            {
                while(dwSubmittedMask);

                hr = pDevice->GetCurrentPosition(&dwPosition);

                if(SUCCEEDED(hr))
                {
                    DbgPrint("FINAL: submitted %lu, position %lu\n", dwSubmittedSize, dwPosition);
                }

                break;
            }
        }
    }

    if(pvBufferData)
    {
        XPhysicalFree(pvBufferData);
    }

    RELEASE(pWaveFile);
    RELEASE(pDevice);

    return hr;
}


/****************************************************************************
 *
 *  RotateVectorAzimuth
 *
 *  Description:
 *      Rotates a vector in a circle or elipse around a point.
 *
 *  Arguments:
 *      LPD3DXVECTOR3 [in/out]: vector.
 *      FLOAT [in]: theta in radians.
 *      FLOAT [in]: circle size in radians.
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
	DWORD					dwAmount
)
{
    static const LPCSTR     pszFile             = "t:\\media\\audio\\pcm\\CleanGuitarArpeggios.wav";//"t:\\media\\audio\\pcm\\heli.wav";
    INT                     nLastX              = -1;
    INT                     nLastY              = -1;
    INT                     nLastZ              = -1;
    FLOAT                   flTheta;
    LPDIRECTSOUND           pDirectSound;
    LPDIRECTSOUNDBUFFER     pBuffer;
    D3DXVECTOR3             vrPreviousPosition;
    D3DXVECTOR3             vrCurrentPosition;
    D3DXVECTOR3             vrVelocity;
    DWORD                   dwStartTime;
    DWORD                   dwCurrentTime;
    FLOAT                   flTimeDelta;
    HRESULT                 hr;
	DWORD					dwCount = 0;

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
    
    hr = ValidateResult(DirectSoundCreate(NULL, &pDirectSound, NULL));

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(pDirectSound->SetPosition(flListenerPositionX, flListenerPositionY, flListenerPositionZ, DS3D_IMMEDIATE));
    }

    //
    // Create the buffer
    //

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(CreateBufferFromFile(pszFile, DSBCAPS_CTRL3D, 0, NULL, &pBuffer));
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
        hr = ValidateResult(PlayBuffer(pBuffer, TRUE));
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

    while( SUCCEEDED(hr) && dwCount < dwAmount )
    {
        //
        // Get the current system time
        //

        while(dwStartTime == (dwCurrentTime = GetTickCount()));

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
            hr = ValidateResult(pDirectSound->CommitDeferredSettings());
        }

        if(SUCCEEDED(hr))
        {
            if(((INT)vrCurrentPosition.x != nLastX) || ((INT)vrCurrentPosition.y != nLastY) || ((INT)vrCurrentPosition.z != nLastZ))
            {
                nLastX = (INT)vrCurrentPosition.x;
                nLastY = (INT)vrCurrentPosition.y;
                nLastZ = (INT)vrCurrentPosition.z;

                DbgPrint("%d %d %d (%d %d %d)\n", nLastX, nLastY, nLastZ, (int)vrVelocity.x, (int)vrVelocity.y, (int)vrVelocity.z);
				dwCount++;
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
    RELEASE(pDirectSound);

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
    DWORD                   dwFlags
)
{
    LPDIRECTSOUNDBUFFER     pBuffer = NULL;
    HRESULT                 hr;

    hr = ValidateResult(CreateBufferFromFile(pszFile, dwFlags, 0, NULL, &pBuffer));

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
    DWORD                   dwSamplsPerSec,
    DWORD                   dwBitsPerSample,
    DWORD                   dwChannelCount
)
{
    LPCWAVEFORMATEX         pwfxFormat  = CreateFormat(dwFormatTag, dwSamplsPerSec, dwBitsPerSample, dwChannelCount);
    LPCSTR                  pszFileName = GetStdWaveFile(pwfxFormat);
    
    return PlayOneShotBuffer(pszFileName, dwFlags);
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
    static const DWORD      dwBufferCount       = NUMELEMS(adwSampleRates) * NUMELEMS(adwChannelCounts) * NUMELEMS(adwBitResolutions);
    LPDIRECTSOUNDBUFFER     pBuffer             = NULL;
    HRESULT                 hr                  = DS_OK;
    DWORD                   i;
    LPCWAVEFORMATEX         pwfx;
    DWORD                   dwFreeSGEs;
	LPCSTR					pFilename;
	WIN32_FIND_DATA			findData;
	HANDLE					hFind;
	BOOL					bValidSize;

    for(i = 0; (i < dwBufferCount) && SUCCEEDED(hr); i++)
    {
        pwfx = CreateFormat(dwFormatTag, 
							adwSampleRates[i % NUMELEMS(adwSampleRates)], 
							adwBitResolutions[i % NUMELEMS(adwBitResolutions)],
							adwChannelCounts[i % NUMELEMS(adwChannelCounts)]);

		// Generate the filename
		pFilename = GetStdWaveFile(pwfx);
		bValidSize = TRUE;

		// Max valid buffer size
		hr = GetFreeSGEs(&dwFreeSGEs);

		// Get the file size
		if(SUCCEEDED(hr))
		{
			// Determine the file size
			hFind = FindFirstFile(pFilename, &findData);
			hr = (hFind == INVALID_HANDLE_VALUE) ? E_FAIL : S_OK;
		}

		// Is the buffer too large to load?
		if(SUCCEEDED(hr))
		{
			FindClose(hFind);
			if((findData.nFileSizeHigh > 0) || (findData.nFileSizeLow > (dwFreeSGEs * 4096)))
				bValidSize = FALSE;
		}

		if(bValidSize)
		{
			hr = PlayOneShotBuffer(0, dwFormatTag, 
								   adwSampleRates[i % NUMELEMS(adwSampleRates)], 
								   adwBitResolutions[i % NUMELEMS(adwBitResolutions)], 
								   adwChannelCounts[i % NUMELEMS(adwChannelCounts)]);
		}
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
    static const DWORD      dwBufferCount               = NUMELEMS(adwSampleRates) * NUMELEMS(adwChannelCounts) * NUMELEMS(adwBitResolutions);
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
	LPCSTR					pFilename;
	WIN32_FIND_DATA			findData;
	HANDLE					hFind;
	BOOL					bValidSize;

    //
    // Get the count of free SGEs
    //

    hr = GetFreeSGEs(&dwFreeSGEs);

    //
    // Calculate the maximum buffer size
    //

    if(SUCCEEDED(hr))
    {
        dwMaxBufferSize = ((dwFreeSGEs - dwBufferCount) * 4096) / dwBufferCount / 4 * 4;
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
		
		// Generate the filename
		pFilename = GetStdWaveFile(pwfx);
		bValidSize = TRUE;

		// Max valid buffer size
		hr = GetFreeSGEs(&dwFreeSGEs);

		// Get the file size
		if(SUCCEEDED(hr))
		{
			// Determine the file size
			hFind = FindFirstFile(pFilename, &findData);
			hr = (hFind == INVALID_HANDLE_VALUE) ? E_FAIL : S_OK;
		}

		// Is the buffer too large to load?
		if(SUCCEEDED(hr))
		{
			FindClose(hFind);
			if((findData.nFileSizeHigh > 0) || (findData.nFileSizeLow > (dwFreeSGEs * 4096)))
				bValidSize = FALSE;
		}
		
		if(bValidSize)
		{
			if(SUCCEEDED(hr))
				hr = CreateStdBuffer(0, pwfx, 0, NULL, &apBuffers[dwActiveBuffers], &dwBufferSize);

			if(SUCCEEDED(hr))
				hr = PlayBuffer(apBuffers[dwActiveBuffers]);

			if(SUCCEEDED(hr))
				dwActiveBuffers++;
		}

		if(SUCCEEDED(hr))
		{
			dwSampleRateIndex		= (dwSampleRateIndex + 1) % NUMELEMS(adwSampleRates);
			dwChannelCountIndex		= (dwChannelCountIndex + 1) % NUMELEMS(adwChannelCounts);
			dwBitResolutionIndex	= (dwBitResolutionIndex + 1) % NUMELEMS(adwBitResolutions);
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
    static const DWORD      dwStreamCount                       = NUMELEMS(adwSampleRates) * NUMELEMS(adwChannelCounts) * NUMELEMS(adwBitResolutions);
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
            dwSampleRateIndex = (dwSampleRateIndex + 1) % NUMELEMS(adwSampleRates);
            dwChannelCountIndex = (dwChannelCountIndex + 1) % NUMELEMS(adwChannelCounts);
            dwBitResolutionIndex = (dwBitResolutionIndex + 1) % NUMELEMS(adwBitResolutions);

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
        SAFECLOSEHANDLE(ahCompletionEvents[i]);
		RELEASE( apStreams[i] );
		FREE( apvStreamData[i] );
    }

	if ( E_OUTOFMEMORY == hr )
	{
		DbgPrint( "CreateStdQueueStream returned E_OUTOFMEMORY. Return HR=S_OK from TestMultipleStreams\n" );
		hr = S_OK;
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
        for(i = 0; i < dwNotifyCount - 1; i++)
        {
            padsbpn[i].dwOffset = i * dwPeriod;
            padsbpn[i].hEventNotify = pahEvents[i];
        }

        padsbpn[dwNotifyCount - 1].dwOffset = DSBPN_OFFSETSTOP;
        padsbpn[dwNotifyCount - 1].hEventNotify = pahEvents[dwNotifyCount - 1];

        hr = ValidateResult(pBuffer->SetNotificationPositions(dwNotifyCount, padsbpn));
    }

    //
    // Start playback
    //

    if(SUCCEEDED(hr))
    {
        hr = PlayBuffer(pBuffer, fLoop);
    }

    //
    // Wait for playback to complete
    //

    while(SUCCEEDED(hr))
    {
        DirectSoundDoWork();
        
        if((i = WaitForMultipleObjects(dwNotifyCount, pahEvents, FALSE, 100)) < dwNotifyCount)
        {
            DbgPrint("Event %lu signaled\n", i);

            if(WAIT_OBJECT_0 + dwNotifyCount - 1 == i)
            {
                break;
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
            SAFECLOSEHANDLE(pahEvents[i]);
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
    DWORD                   dwPacketSize
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
        hr = CreateQueuedStreamFromFile(pszFile, dwFlags, 0, &dwPacketSize, hCompletionEvent, NULL, &pStream, &pvDataBuffer);
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
    SAFECLOSEHANDLE(hCompletionEvent);

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
    DWORD                   dwPacketSize
)
{
    LPCWAVEFORMATEX         pwfxFormat  = CreateFormat(dwFormatTag, dwSamplesPerSec, dwBitsPerSample, dwChannelCount);
    LPCSTR                  pszFileName = GetStdWaveFile(pwfxFormat);
    
    return PlayOneShotStream(pszFileName, dwFlags, dwPacketSize);
}


/****************************************************************************
 *
 *  TestLoopRegion
 *
 *  Description:
 *      Tests buffer loop regions.
 *
 *  Arguments:
 *      DWORD [in]: format tag.
 *      DWORD [in]: sampling rate.
 *      DWORD [in]: bit resolution.
 *      DWORD [in]: channel count.
 *      FLOAT [in]: loop start position (0 to 1).
 *      FLOAT [in]: loop end position (0 to 1).
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "TestLoopRegion"

HRESULT
TestLoopRegion
(
    DWORD                   dwFormatTag,
    DWORD                   dwSampleRate,
    DWORD                   dwBitsPerSample,
    DWORD                   dwChannelCount,
    FLOAT                   flLoopStart,
    FLOAT                   flLoopEnd
)
{
    LPCWAVEFORMATEX         pwfxFormat  = CreateFormat(dwFormatTag, dwSampleRate, dwBitsPerSample, dwChannelCount);
    LPDIRECTSOUNDBUFFER     pBuffer     = NULL;
    DWORD                   dwDuration  = 0;
    DWORD                   dwLoopStart;
    DWORD                   dwLoopLength;
    HRESULT                 hr;

    hr = CreateStdBuffer(0, pwfxFormat, 0, NULL, &pBuffer, &dwDuration);

    if(SUCCEEDED(hr))
    {
        dwLoopStart = (DWORD)((FLOAT)dwDuration * flLoopStart) / pwfxFormat->nBlockAlign * pwfxFormat->nBlockAlign;
        dwLoopLength = ((DWORD)((FLOAT)dwDuration * flLoopEnd) / pwfxFormat->nBlockAlign * pwfxFormat->nBlockAlign) - dwLoopStart;

        DbgPrint("Setting loop start offset to %lu and loop end to %lu\n", dwLoopStart, dwLoopStart + dwLoopLength);
        
        hr = ValidateResult(pBuffer->SetLoopRegion(dwLoopStart, dwLoopLength));

    }

    if(SUCCEEDED(hr))
    {
        hr = PlayBuffer(pBuffer, TRUE);
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
    static const DWORD      dwBufferCount               = NUMELEMS(adwSampleRates) * NUMELEMS(adwChannelCounts) * NUMELEMS(adwBitResolutions);
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
    DWORD                   i, z;
	DSMIXBINVOLUMEPAIR		mixBinVolumePair;
	DSMIXBINS				mixBins;

    //
    // Create output buffers
    //

    ZeroMemory(&dsbd, sizeof(dsbd));

    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = fMixIn ? DSBCAPS_MIXIN : DSBCAPS_FXIN;

    for(i = 0; (i < NUMELEMS(apOutputBuffers)) && SUCCEEDED(hr); i++)
    {
		mixBinVolumePair.dwMixBin	= i;
		mixBinVolumePair.lVolume	= 0;
		mixBins.dwMixBinCount		= 1;
		mixBins.lpMixBinVolumePairs	= &mixBinVolumePair;
        dsbd.lpMixBins				= &mixBins;

        if(!fMixIn)
        {
            dsbd.dwInputMixBin = DSMIXBIN_FXSEND_5 << i;
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
        dwMaxBufferSize = ((dwFreeSGEs - dwBufferCount) * 4096) / dwBufferCount / 4 * 4;
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
        
        hr = CreateStdBuffer(0, pwfx, 0, apOutputBuffers[dwActiveBuffers % NUMELEMS(apOutputBuffers)], &apBuffers[dwActiveBuffers], &dwBufferSize);

        if(SUCCEEDED(hr))
        {
            hr = PlayBuffer(apBuffers[dwActiveBuffers]);
        }

        if(SUCCEEDED(hr))
        {
            dwSampleRateIndex = (dwSampleRateIndex + 1) % NUMELEMS(adwSampleRates);
            dwChannelCountIndex = (dwChannelCountIndex + 1) % NUMELEMS(adwChannelCounts);
            dwBitResolutionIndex = (dwBitResolutionIndex + 1) % NUMELEMS(adwBitResolutions);

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

    for(i = 0; i < NUMELEMS(apOutputBuffers); i++)
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
        hr = PlayBuffer(pBuffer, TRUE);
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
	typedef enum
	{
		TESTBINMASK_SPEAKER_MASK,
		TESTBINMASK_FRONT_LEFT,
		TESTBINMASK_FRONT_RIGHT,
		TESTBINMASK_FRONT_CENTER,
		TESTBINMASK_BACK_LEFT,
		TESTBINMASK_BACK_RIGHT,
		TESTBINMASK_LOW_FREQUENCY
	} TESTBINMASK;

    typedef struct
    {
        LPCSTR		pszDescription;
        TESTBINMASK	dwMixBin;
    } TESTMIXBINS;

    static const TESTMIXBINS    aMixBins[] =
    {
        { "All speakers",       TESTBINMASK_SPEAKER_MASK },
        { "Front left",         TESTBINMASK_FRONT_LEFT },
        { "Front right",        TESTBINMASK_FRONT_RIGHT },
        { "Front center",       TESTBINMASK_FRONT_CENTER },
        { "Back left",          TESTBINMASK_BACK_LEFT },
        { "Back right",         TESTBINMASK_BACK_RIGHT },
        { "LFE",                TESTBINMASK_LOW_FREQUENCY },
    };
    
    static const DWORD          dwDuration  = 5000;
    LPDIRECTSOUNDBUFFER         pBuffer     = NULL;
    HRESULT                     hr;
    DWORD                       i;
	DSMIXBINVOLUMEPAIR			mixBinVolumePairs[6];
	DSMIXBINS					mixBins;
	
    hr = CreateStdBuffer(0, CreateFormat(dwFormatTag, dwSamplesPerSec, dwBitsPerSample, 1), 0, NULL, &pBuffer);

    for(i = 0; (i < NUMELEMS(aMixBins)) && SUCCEEDED(hr); i++)
    {
        DbgPrint( "%s\n", aMixBins[i].pszDescription);

		switch(aMixBins[i].dwMixBin)
		{
			case TESTBINMASK_SPEAKER_MASK:
				mixBinVolumePairs[0].dwMixBin	= DSMIXBIN_FRONT_LEFT;
				mixBinVolumePairs[0].lVolume	= 0;
				mixBinVolumePairs[1].dwMixBin	= DSMIXBIN_FRONT_RIGHT;
				mixBinVolumePairs[1].lVolume	= 0;
				mixBinVolumePairs[2].dwMixBin	= DSMIXBIN_FRONT_CENTER;
				mixBinVolumePairs[2].lVolume	= 0;
				mixBinVolumePairs[3].dwMixBin	= DSMIXBIN_LOW_FREQUENCY;
				mixBinVolumePairs[3].lVolume	= 0;
				mixBinVolumePairs[4].dwMixBin	= DSMIXBIN_BACK_LEFT;
				mixBinVolumePairs[4].lVolume	= 0;
				mixBinVolumePairs[5].dwMixBin	= DSMIXBIN_BACK_RIGHT;
				mixBinVolumePairs[5].lVolume	= 0;
				mixBins.dwMixBinCount			= 6;
				mixBins.lpMixBinVolumePairs		= mixBinVolumePairs;
				break;
			case TESTBINMASK_FRONT_LEFT:
				mixBinVolumePairs[0].dwMixBin	= DSMIXBIN_FRONT_LEFT;
				mixBinVolumePairs[0].lVolume	= 0;
				mixBins.dwMixBinCount			= 1;
				mixBins.lpMixBinVolumePairs		= mixBinVolumePairs;
				break;
			case TESTBINMASK_FRONT_RIGHT:
				mixBinVolumePairs[0].dwMixBin	= DSMIXBIN_FRONT_RIGHT;
				mixBinVolumePairs[0].lVolume	= 0;
				mixBins.dwMixBinCount			= 1;
				mixBins.lpMixBinVolumePairs		= mixBinVolumePairs;
				break;
			case TESTBINMASK_FRONT_CENTER:
				mixBinVolumePairs[0].dwMixBin	= DSMIXBIN_FRONT_CENTER;
				mixBinVolumePairs[0].lVolume	= 0;
				mixBins.dwMixBinCount			= 1;
				mixBins.lpMixBinVolumePairs		= mixBinVolumePairs;
				break;
			case TESTBINMASK_BACK_LEFT:
				mixBinVolumePairs[0].dwMixBin	= DSMIXBIN_BACK_LEFT;
				mixBinVolumePairs[0].lVolume	= 0;
				mixBins.dwMixBinCount			= 1;
				mixBins.lpMixBinVolumePairs		= mixBinVolumePairs;
				break;
			case TESTBINMASK_BACK_RIGHT:
				mixBinVolumePairs[0].dwMixBin	= DSMIXBIN_BACK_RIGHT;
				mixBinVolumePairs[0].lVolume	= 0;
				mixBins.dwMixBinCount			= 1;
				mixBins.lpMixBinVolumePairs		= mixBinVolumePairs;
				break;
			case TESTBINMASK_LOW_FREQUENCY:
				mixBinVolumePairs[0].dwMixBin	= DSMIXBIN_LOW_FREQUENCY;
				mixBinVolumePairs[0].lVolume	= 0;
				mixBins.dwMixBinCount			= 1;
				mixBins.lpMixBinVolumePairs		= mixBinVolumePairs;
				break;
		}

        hr = ValidateResult(pBuffer->SetMixBins(&mixBins));

        if(SUCCEEDED(hr))
        {
            hr = PlayBuffer(pBuffer, TRUE);
        }

        if(SUCCEEDED(hr))
        {
            Sleep(dwDuration);
        }
    }

    RELEASE(pBuffer);

    return hr;
}


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

    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 48000;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = 2 * 16 / 8;
    wfx.nAvgBytesPerSec = 48000 * wfx.nBlockAlign;
    wfx.cbSize = 0;

    ZeroMemory(&dsbd, sizeof(dsbd));

    dsbd.dwSize = sizeof(dsbd);
    dsbd.lpwfxFormat = &wfx;

    dwBufferSize = -1;
    
    hr = ClampBufferSize(&dwBufferSize,wfx.nBlockAlign);

    if(SUCCEEDED(hr))
    {
        hr = MALLOC(pvBufferData, BYTE, dwBufferSize);
    }

    if(SUCCEEDED(hr))
    {
        GenerateTone16(pvBufferData, dwBufferSize, wfx.nSamplesPerSec, wfx.nChannels, 500);
    }

    for(i = 0; (i < NUMELEMS(apBuffers)) && SUCCEEDED(hr); i++)
    {
        hr = ValidateResult(DirectSoundCreateBuffer(&dsbd, &apBuffers[i]));

        if(SUCCEEDED(hr))
        {
            hr = ValidateResult(apBuffers[i]->SetBufferData(pvBufferData, dwBufferSize));
        }

        if(SUCCEEDED(hr))
        {
            hr = PlayBuffer(apBuffers[i], TRUE);
        }

        if(SUCCEEDED(hr))
        {
            hr = StopBuffer(apBuffers[i], 0, (5000 * i) + 10000);
        }
    }

    for(i = 0; (i < NUMELEMS(apBuffers)) && SUCCEEDED(hr); i++)
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
 *  TestAllVoices
 *
 *  Description:
 *      Creates all voices.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestAllVoices"

HRESULT
TestAllVoices
(
    void
)
{
    static const DWORD          dwBufferSize    = 4096;
    LPVOID                      pvBufferData    = NULL;
    LPDIRECTSOUNDBUFFER         apBuffers[256]  = { NULL };
    DSCAPS                      dsc;
    DSBUFFERDESC                dsbd;
    WAVEFORMATEX                wfx;
    HRESULT                     hr;
    DWORD                       i;
	DWORD						index;

    ZeroMemory(&dsbd, sizeof(dsbd));

    dsbd.dwSize			= sizeof(dsbd);
    dsbd.lpwfxFormat	= &wfx;

    wfx.wFormatTag		= WAVE_FORMAT_PCM;
    wfx.nChannels		=  1;
    wfx.nSamplesPerSec	= dwBufferSize / 2;
    wfx.wBitsPerSample	= 16;
    wfx.nBlockAlign		= 1 * 16 / 8;
    wfx.nAvgBytesPerSec	= dwBufferSize / 2 * wfx.nBlockAlign;
    wfx.cbSize			= 0;

    hr = MALLOC(pvBufferData, BYTE, dwBufferSize);

    if(SUCCEEDED(hr))
        GenerateTone16(pvBufferData, dwBufferSize, wfx.nSamplesPerSec, wfx.nChannels, 500);

    if(SUCCEEDED(hr))
        hr = GetDirectSoundCaps(&dsc);

	// 2D voices
    for(index=0, i=0; (i < dsc.dwFree2DBuffers) && SUCCEEDED(hr); i++, ++index)
    {
        hr = ValidateResult(DirectSoundCreateBuffer(&dsbd, &apBuffers[index]));

        if(SUCCEEDED(hr))
            hr = ValidateResult(apBuffers[index]->SetBufferData(pvBufferData, dwBufferSize));

        if(SUCCEEDED(hr))
            hr = PlayBuffer(apBuffers[index], TRUE);
    }

	// 3D voices
    dsbd.dwFlags |= DSBCAPS_CTRL3D;
    for(i=0; (i < dsc.dwFree3DBuffers) && SUCCEEDED(hr); i++, ++index)
    {
        hr = ValidateResult(DirectSoundCreateBuffer(&dsbd, &apBuffers[index]));

        if(SUCCEEDED(hr))
            hr = ValidateResult(apBuffers[index]->SetBufferData(pvBufferData, dwBufferSize));

        if(SUCCEEDED(hr))
            hr = PlayBuffer(apBuffers[index], TRUE);
    }

    if(SUCCEEDED(hr))
        Sleep(5000);

    for(i = 0; i < NUMELEMS(apBuffers); i++)
    {
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
        hr = PlayBuffer(pBuffer, TRUE);
    }

    if(SUCCEEDED(hr))
    {
        Sleep(1);
        
        hr = ValidateResult(pBuffer->GetCurrentPosition(&dwPlay, &dwWrite));
    }

    if(SUCCEEDED(hr))
    {
        DbgPrint("initial play:  play %lu write %lu duration %lu\n", dwPlay, dwWrite, dwDuration);
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
        DbgPrint("manual setposition:  play %lu write %lu duration %lu\n", dwPlay, dwWrite, dwDuration);
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
        DbgPrint("forced stop:  play %lu write %lu duration %lu\n", dwPlay, dwWrite, dwDuration);
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
        DbgPrint("second play:  play %lu write %lu duration %lu\n", dwPlay, dwWrite, dwDuration);
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
        DbgPrint("natural stop:  play %lu write %lu duration %lu\n", dwPlay, dwWrite, dwDuration);
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
        DbgPrint("third play:  play %lu write %lu duration %lu\n", dwPlay, dwWrite, dwDuration);
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
        PlayBuffer(pBuffer, TRUE);
    }

    if(SUCCEEDED(hr))
    {
        for(dwFrequency = 0; (dwFrequency < NUMELEMS(adwFrequencies)) && SUCCEEDED(hr); dwFrequency++)
        {
            DbgPrint("Frequency %lu\n", adwFrequencies[dwFrequency]);

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
        hr = PlayBuffer(pBuffer, TRUE);
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

#undef DPF_FNAME
#define DPF_FNAME "TestHeadroom"

HRESULT
TestHeadroom
(
    void
)
{
    static const DWORD      dwSleep         = 2000;
    LPDIRECTSOUND           pDirectSound    = NULL;
    LPDIRECTSOUNDBUFFER     pBuffer         = NULL;
    HRESULT                 hr;
    int                     i;

    hr = ValidateResult(DirectSoundCreate(NULL, &pDirectSound, NULL));

    if(SUCCEEDED(hr))
    {
        hr = CreateToneBuffer(0, 8000, 16, 1, 500, 0, 0, &pBuffer);
    }

    if(SUCCEEDED(hr))
    {
        hr = PlayBuffer(pBuffer, TRUE);
    }

    for(i = DSBHEADROOM_MAX; SUCCEEDED(hr) && (i >= DSBHEADROOM_MIN); i -= 1000)
    {
        DbgPrint("%lu voice headroom\n", i);

        hr = ValidateResult(pBuffer->SetHeadroom(i));

        if(SUCCEEDED(hr))
        {
            Sleep(dwSleep);
        }
    }

    for(i = DSHEADROOM_MIN; SUCCEEDED(hr) && (i <= DSHEADROOM_MAX); i++)
    {
        DbgPrint("%lu mixbin headroom\n", i);

        hr = ValidateResult(pDirectSound->SetMixBinHeadroom(DSMIXBIN_FRONT_LEFT, i));

        if(SUCCEEDED(hr))
        {
            Sleep(dwSleep);
        }
    }

    RELEASE(pBuffer);
    RELEASE(pDirectSound);

    return hr;
}


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

	DSMIXBINVOLUMEPAIR				mixBinVolumePairs[7];
	DSMIXBINS						mixBins;
    static const DWORD              dwDelay         = 10000;
    LPDIRECTSOUND                   pDirectSound    = NULL;
    LPDIRECTSOUNDBUFFER             pBuffer         = NULL;
    HRESULT                         hr;
    DWORD                           i;

	mixBinVolumePairs[0].dwMixBin	= DSMIXBIN_FRONT_LEFT;
	mixBinVolumePairs[0].lVolume	= 0;
	mixBinVolumePairs[1].dwMixBin	= DSMIXBIN_FRONT_RIGHT;
	mixBinVolumePairs[1].lVolume	= 0;
	mixBinVolumePairs[2].dwMixBin	= DSMIXBIN_FRONT_CENTER;
	mixBinVolumePairs[2].lVolume	= 0;
	mixBinVolumePairs[3].dwMixBin	= DSMIXBIN_LOW_FREQUENCY;
	mixBinVolumePairs[3].lVolume	= 0;
	mixBinVolumePairs[4].dwMixBin	= DSMIXBIN_BACK_LEFT;
	mixBinVolumePairs[4].lVolume	= 0;
	mixBinVolumePairs[5].dwMixBin	= DSMIXBIN_BACK_RIGHT;
	mixBinVolumePairs[5].lVolume	= 0;
	mixBinVolumePairs[6].dwMixBin	= DSMIXBIN_I3DL2;
	mixBinVolumePairs[6].lVolume	= 0;
	mixBins.dwMixBinCount			= 7;
	mixBins.lpMixBinVolumePairs		= mixBinVolumePairs;
	
    hr = ValidateResult(DirectSoundCreate(NULL, &pDirectSound, NULL));

    if(SUCCEEDED(hr))
    {
        hr = CreateBufferFromFile("t:\\media\\audio\\pcm\\CleanGuitarArpeggios.wav", 0, &mixBins, NULL, &pBuffer);
    }

    for(i = 0; SUCCEEDED(hr) && (i < NUMELEMS(Environments)); i++)
    {
        DbgPrint("%s\n", Environments[i].pszName);

        hr = ValidateResult(pDirectSound->SetI3DL2Listener(&Environments[i].Properties, DS3D_IMMEDIATE));

        if(SUCCEEDED(hr))
        {
            hr = PlayBuffer(pBuffer, TRUE);
        }

        if(SUCCEEDED(hr))
        {
            Sleep(dwDelay);
        }
    }

    RELEASE(pBuffer);
    RELEASE(pDirectSound);

    return hr;
}


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

    hr = LoadStdWaveFile(pwfxSrc, &pWaveFile);

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(pWaveFile->GetLength(&dwSrcDataSize));
    }

    if(SUCCEEDED(hr))
    {
        hr = MALLOC(pvSrcData, BYTE, dwSrcDataSize);
    }

    if(SUCCEEDED(hr))
    {
        hr = ReadXMO(pWaveFile, pvSrcData, dwSrcDataSize, &dwSrcDataSize);
    }

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

            DbgPrint("Converting from %lu channel to %lu channel\n", pwfxSrc->nChannels, wfxDst.nChannels);
            
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
                hr = ClampBufferSize(&dwDstDataSize,wfxDst.nBlockAlign);
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

            RELEASE(pBuffer);
	        FREE(pvDstData);
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
        hr = ValidateResult(PlayBuffer(pBuffer, TRUE));
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
 
#undef DPF_FNAME
#define DPF_FNAME "CompareVolume"

HRESULT
CompareVolume
(
    FLOAT                   flDistance,
	DWORD					dwNumTimes
)
{
    static const LONG       lExtra  = -600;
    const LONG              lVolume = (LONG)(-2000.0f * log10f(flDistance)) + lExtra;
    LPDIRECTSOUNDBUFFER     pBuffer = NULL;
    BOOL                    f3d     = FALSE;
    HRESULT                 hr      = DS_OK;
	DWORD					dwCount = 0;
	DSMIXBINVOLUMEPAIR		mixBinVolumePair[2];
	DSMIXBINS				mixBins;

	mixBinVolumePair[0].dwMixBin	= DSMIXBIN_FRONT_LEFT;
	mixBinVolumePair[0].lVolume		= 0;
	mixBinVolumePair[1].dwMixBin	= DSMIXBIN_FRONT_RIGHT;
	mixBinVolumePair[1].lVolume		= 0;
	mixBins.dwMixBinCount			= 2;
	mixBins.lpMixBinVolumePairs		= mixBinVolumePair;

    while(SUCCEEDED(hr) && dwCount < dwNumTimes)
    {
        DbgPrint(f3d ? "3D\n" : "2D\n");
        
        hr = ValidateResult(CreateToneBuffer(f3d ? DSBCAPS_CTRL3D : 0, 8000, 16, 1, 500, 0, f3d ? NULL : &mixBins, &pBuffer));

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
            hr = ValidateResult(PlayBuffer(pBuffer, TRUE));
        }

        if(SUCCEEDED(hr))
        {
            hr = ValidateResult(StopBuffer(pBuffer, 0, 2000, FALSE, TRUE));
        }

        RELEASE(pBuffer);

        f3d = !f3d;

		dwCount++;
    }

    RELEASE(pBuffer);

    return hr;
}


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
        hr = ValidateResult(PlayBuffer(pBuffer, TRUE));
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
            if((XMEDIAPACKET_STATUS_SUCCESS != padwStatus[i]) && 
			   (XMEDIAPACKET_STATUS_FLUSHED != padwStatus[i]))
            {
                if(padwStatus[i] == XMEDIAPACKET_STATUS_PENDING)
	                DbgPrint("Packet %lu status: XMEDIAPACKET_STATUS_PENDING\n", i);
				else if(padwStatus[i] == XMEDIAPACKET_STATUS_FAILURE)
	                DbgPrint("Packet %lu status: XMEDIAPACKET_STATUS_FAILURE\n", i);
				else
					DbgPrint("Packet %lu status: %x\n", i, padwStatus[i]);
                hr = E_FAIL;
            }
        }
    }

    RELEASE(pStream);
    FREE(padwStatus);
    FREE(pvAudioData);

    return hr;
}

//------------------------------------------------------------------------------
//	TestFlushEx1
//------------------------------------------------------------------------------
#undef DPF_FNAME
#define DPF_FNAME "TestFlushEx"
HRESULT
TestFlushEx1(void)
/*++

Routine Description:

	Tests the FlushEx method on streams: DSSTREAMFLUSHEX_ASYNC

Arguments:

	None

Return Value:

	HRESULT: COM result code

--*/
{
    const LPCWAVEFORMATEX	pwfxFormat		= CreateFormat(WAVE_FORMAT_PCM, 44100, 16, 2);
    LPDIRECTSOUNDSTREAM		pStream			= NULL;
    DWORD					dwPacketSize	= 0x1000;
    LPVOID					pvAudioData		= NULL;
    DWORD					dwAudioDataSize	= NULL;
    DWORD					dwPacketCount	= 0;
    LPDWORD					padwStatus		= NULL;
    XMEDIAPACKET			xmp;
    HRESULT					hr;
    DWORD					i;

    hr = ValidateResult(CreateStdStream(0, pwfxFormat, 0, &dwPacketSize, NULL,
										NULL, NULL, &pStream, &pvAudioData, 
										&dwAudioDataSize, &dwPacketCount));

    if(SUCCEEDED(hr))
    {
        hr = MALLOC(padwStatus, DWORD, dwPacketCount);
    }

    if(SUCCEEDED(hr))
    {
        ZeroMemory(&xmp, sizeof(xmp));

        xmp.pvBuffer	= pvAudioData;
        xmp.dwMaxSize	= dwPacketSize;
        xmp.pdwStatus	= padwStatus;
        
        for(i = 0; SUCCEEDED(hr) && (i < dwPacketCount); i++)
        {
            if(i + 1 == dwPacketCount)
            {
                xmp.dwMaxSize = dwAudioDataSize - (dwPacketSize * 
												   (dwPacketCount - 1));
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
        hr = ValidateResult(pStream->FlushEx(0, DSSTREAMFLUSHEX_ASYNC));
		if(SUCCEEDED(hr))
		{
			do
			{
				DirectSoundDoWork();
				hr = ValidateResult(pStream->GetStatus(&i));
				Sleep(20);
			}
			while(SUCCEEDED(hr) && (i == DSSTREAMSTATUS_PLAYING));
		}
    }

    if(SUCCEEDED(hr))
    {
        for(i = 0; i < dwPacketCount; i++)
        {
            if((XMEDIAPACKET_STATUS_SUCCESS != padwStatus[i]) && 
			   (XMEDIAPACKET_STATUS_FLUSHED != padwStatus[i]))
            {
                if(padwStatus[i] == XMEDIAPACKET_STATUS_PENDING)
	                DbgPrint("Packet %lu status: XMEDIAPACKET_STATUS_PENDING\n", i);
				else if(padwStatus[i] == XMEDIAPACKET_STATUS_FAILURE)
	                DbgPrint("Packet %lu status: XMEDIAPACKET_STATUS_FAILURE\n", i);
				else
					DbgPrint("Packet %lu status: %x\n", i, padwStatus[i]);
                hr = E_FAIL;
            }
        }
    }

    RELEASE(pStream);
    FREE(padwStatus);
    FREE(pvAudioData);

    return hr;
}

//------------------------------------------------------------------------------
//	TestFlushEx2
//------------------------------------------------------------------------------
#undef DPF_FNAME
#define DPF_FNAME "TestFlushEx"
HRESULT
TestFlushEx2(void)
/*++

Routine Description:

	Tests the FlushEx method on streams

Arguments:

	None

Return Value:

	HRESULT: COM result code

--*/
{
    const LPCWAVEFORMATEX	pwfxFormat		= CreateFormat(WAVE_FORMAT_PCM, 44100, 16, 2);
    LPDIRECTSOUNDSTREAM		pStream			= NULL;
    DWORD					dwPacketSize	= 0x1000;
    LPVOID					pvAudioData		= NULL;
    DWORD					dwAudioDataSize	= NULL;
    DWORD					dwPacketCount	= 0;
    LPDWORD					padwStatus		= NULL;
    XMEDIAPACKET			xmp;
    HRESULT					hr;
    DWORD					i;

    hr = ValidateResult(CreateStdStream(0, pwfxFormat, 0, &dwPacketSize, NULL,
										NULL, NULL, &pStream, &pvAudioData, 
										&dwAudioDataSize, &dwPacketCount));

    if(SUCCEEDED(hr))
    {
        hr = MALLOC(padwStatus, DWORD, dwPacketCount);
    }

    if(SUCCEEDED(hr))
    {
        ZeroMemory(&xmp, sizeof(xmp));

        xmp.pvBuffer	= pvAudioData;
        xmp.dwMaxSize	= dwPacketSize;
        xmp.pdwStatus	= padwStatus;
        
        for(i = 0; SUCCEEDED(hr) && (i < dwPacketCount); i++)
        {
            if(i + 1 == dwPacketCount)
            {
                xmp.dwMaxSize = dwAudioDataSize - (dwPacketSize * 
												   (dwPacketCount - 1));
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

        hr = ValidateResult(pStream->FlushEx(0, DSSTREAMFLUSHEX_ASYNC | DSSTREAMFLUSHEX_ENVELOPE));
		if(SUCCEEDED(hr))
		{
			do
			{
				DirectSoundDoWork();
				hr = pStream->GetStatus(&i);
				Sleep(20);
			}
			while(SUCCEEDED(hr) && (i == DSSTREAMSTATUS_PLAYING));
		}
    }

    if(SUCCEEDED(hr))
    {
        for(i = 0; i < dwPacketCount; i++)
        {
            if((XMEDIAPACKET_STATUS_SUCCESS != padwStatus[i]) && 
			   (XMEDIAPACKET_STATUS_FLUSHED != padwStatus[i]))
            {
                if(padwStatus[i] == XMEDIAPACKET_STATUS_PENDING)
	                DbgPrint("Packet %lu status: XMEDIAPACKET_STATUS_PENDING\n", i);
				else if(padwStatus[i] == XMEDIAPACKET_STATUS_FAILURE)
	                DbgPrint("Packet %lu status: XMEDIAPACKET_STATUS_FAILURE\n", i);
				else
					DbgPrint("Packet %lu status: %x\n", i, padwStatus[i]);
                hr = E_FAIL;
            }
        }
    }

    RELEASE(pStream);
    FREE(padwStatus);
    FREE(pvAudioData);

    return hr;
}

//------------------------------------------------------------------------------
//	TestFlushEx3
//------------------------------------------------------------------------------
#undef DPF_FNAME
#define DPF_FNAME "TestFlushEx"
HRESULT
TestFlushEx3(void)
/*++

Routine Description:

	Tests the FlushEx method on streams

Arguments:

	None

Return Value:

	HRESULT: COM result code

--*/
{
    const LPCWAVEFORMATEX	pwfxFormat		= CreateFormat(WAVE_FORMAT_PCM, 44100, 16, 2);
    LPDIRECTSOUNDSTREAM		pStream			= NULL;
    DWORD					dwPacketSize	= 0x1000;
    LPVOID					pvAudioData		= NULL;
    DWORD					dwAudioDataSize	= NULL;
    DWORD					dwPacketCount	= 0;
    LPDWORD					padwStatus		= NULL;
    XMEDIAPACKET			xmp;
    HRESULT					hr;
    DWORD					i;

    hr = ValidateResult(CreateStdStream(0, pwfxFormat, 0, &dwPacketSize, NULL,
										NULL, NULL, &pStream, &pvAudioData, 
										&dwAudioDataSize, &dwPacketCount));

    if(SUCCEEDED(hr))
    {
        hr = MALLOC(padwStatus, DWORD, dwPacketCount);
    }

    if(SUCCEEDED(hr))
    {
        ZeroMemory(&xmp, sizeof(xmp));

        xmp.pvBuffer	= pvAudioData;
        xmp.dwMaxSize	= dwPacketSize;
        xmp.pdwStatus	= padwStatus;
        
        for(i = 0; SUCCEEDED(hr) && (i < dwPacketCount); i++)
        {
            if(i + 1 == dwPacketCount)
            {
                xmp.dwMaxSize = dwAudioDataSize - (dwPacketSize * 
												   (dwPacketCount - 1));
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

        hr = ValidateResult(pStream->FlushEx(0, DSSTREAMFLUSHEX_IMMEDIATE));
    }

    if(SUCCEEDED(hr))
    {
        for(i = 0; i < dwPacketCount; i++)
        {
            if((XMEDIAPACKET_STATUS_SUCCESS != padwStatus[i]) && 
			   (XMEDIAPACKET_STATUS_FLUSHED != padwStatus[i]))
            {
                if(padwStatus[i] == XMEDIAPACKET_STATUS_PENDING)
	                DbgPrint("Packet %lu status: XMEDIAPACKET_STATUS_PENDING\n", i);
				else if(padwStatus[i] == XMEDIAPACKET_STATUS_FAILURE)
	                DbgPrint("Packet %lu status: XMEDIAPACKET_STATUS_FAILURE\n", i);
				else
					DbgPrint("Packet %lu status: %x\n", i, padwStatus[i]);
                hr = E_FAIL;
            }
        }
    }

    RELEASE(pStream);
    FREE(padwStatus);
    FREE(pvAudioData);

    return hr;
}

//------------------------------------------------------------------------------
//	TestFlushEx
//------------------------------------------------------------------------------
#undef DPF_FNAME
#define DPF_FNAME "TestFlushEx"
HRESULT
TestFlushEx(void)
/*++

Routine Description:

	Tests the FlushEx method on streams

Arguments:

	None

Return Value:

	HRESULT: COM result code

--*/
{
	HRESULT	hr1;
	HRESULT	hr2;
	HRESULT	hr3;

	hr1 = ValidateResult(TestFlushEx1());
	hr2 = ValidateResult(TestFlushEx2());
	hr3 = ValidateResult(TestFlushEx3());

	if(FAILED(hr1) || FAILED(hr2) || FAILED(hr3))
		return E_FAIL;
    return S_OK;
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
        for(i = 0; (i < NUMELEMS(alPitch)) && SUCCEEDED(hr); i++)
        {
            DbgPrint("Pitch %d\n", alPitch[i]);
            
            hr = ValidateResult(pBuffer->SetPitch(alPitch[i]));

            if(SUCCEEDED(hr))
            {
                hr = ValidateResult(PlayBuffer(pBuffer, TRUE));
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
    DWORD                   dwTestLength
)
{
    LPDIRECTSOUNDBUFFER     apSourceBuffers[256]    = { NULL };
    LPDIRECTSOUNDBUFFER     pDestBuffer             = NULL;
    DWORD                   dwPlayCount             = 0;
    DWORD                   dwStopCount             = 0;
    DWORD                   dwNaturalStopCount      = 0;
    DWORD                   dwSubmixCount           = 0;
    DWORD                   dwLoopCount             = 0;
    DWORD                   dwSourceCount;
    DWORD                   dwStartTime;
    DWORD                   dwCurrentTime;
    DSCAPS                  dsc;
    DSBUFFERDESC            dsbd;
    DWORD                   dwStatus;
    HRESULT                 hr;
    DWORD                   i;
	DWORD					index;
	BOOL					b2DBuffer[256];

    //
    // Create the submix buffer
    //

    ZeroMemory(&dsbd, sizeof(dsbd));
    
    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = DSBCAPS_MIXIN;

    hr = ValidateResult(DirectSoundCreateBuffer(&dsbd, &pDestBuffer));

    //
    // Create source buffers
    //
    
    if(SUCCEEDED(hr))
        hr = GetDirectSoundCaps(&dsc);

    if(SUCCEEDED(hr))
    {
		dwSourceCount = dsc.dwFree2DBuffers + dsc.dwFree3DBuffers;

		// 2D buffers
        for(index=0, i = 0; (i < dsc.dwFree2DBuffers) && SUCCEEDED(hr); i++, ++index)
		{
            hr = ValidateResult(CreateToneBuffer(0, 32000, 16, 1, 500, 0, 0, &apSourceBuffers[index]));
			b2DBuffer[index] = TRUE;
		}

		// 3D buffers
        for(i = 0; (i < dsc.dwFree3DBuffers) && SUCCEEDED(hr); i++, ++index)
		{
            hr = ValidateResult(CreateToneBuffer(DSBCAPS_CTRL3D, 32000, 16, 1, 500, 0, 0, &apSourceBuffers[index]));
			b2DBuffer[index] = FALSE;
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
        //
        // Check the current time
        //
        
        if(!((dwCurrentTime = GetTickCount()) % 2000))
        {
            DbgPrint("%lu plays (%lu submixing, %lu looping), %lu natural stops, %lu forced\n", dwPlayCount, dwSubmixCount, dwLoopCount, dwStopCount, dwNaturalStopCount);

            dwPlayCount = 0;
            dwStopCount = 0;
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
        // Pick a random buffer
        //

        i = rand() % dwSourceCount;

        hr = ValidateResult(apSourceBuffers[i]->GetStatus(&dwStatus));

        //
        // Should we play or stop?
        //

        if(SUCCEEDED(hr))
        {
            if(rand() % 2)
            {
                dwPlayCount++;
            
                //
                // Submix or not?
                //
            
                if(!(dwStatus & DSBSTATUS_PLAYING))
                {
                    if(b2DBuffer[i] && (rand() % 2))
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
            
                if(dwStatus & DSBSTATUS_PLAYING)
                {
                    dwStopCount++;

                    hr = ValidateResult(apSourceBuffers[i]->Stop());
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
    LONG	lPitch;
    DWORD	i;
    HRESULT	hr	= DS_OK;

	for(i=DSBFREQUENCY_MIN; i<=DSBFREQUENCY_MAX; ++i)
	{
		lPitch = XAudioCalculatePitch(i);
		if((lPitch < DSBPITCH_MIN) || (lPitch > DSBPITCH_MAX))
		{
			DbgPrint("Out of range Pitch: %ld, Frequency: %lu\n", lPitch, i);
			hr = E_FAIL;
		}
	}

    return DS_OK;
}


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
 *  DsoundDRT_StartTest
 *
 *  Description:
 *      The Harness Entry into the DsoundDRT tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI DsoundDRT_StartTest( HANDLE LogHandle )
{
    HRESULT			hr              = S_OK;
    LPDIRECTSOUND	pDirectSound    = NULL;
	MEDIAFILEDESC	media[] = 
	{
		{"dsp/dsstdfx.bin", "t:\\media\\dspcode", COPY_IF_NEWER },
		{"wav/CleanGuitarArpeggios.wav", "t:\\media\\audio\\pcm", COPY_IF_NOT_EXIST },
		{"wav/(\\d+)k(\\d+)b[m s].wav", "t:\\media\\audio\\pcm", COPY_IF_NOT_EXIST },
		{"wav/adpcm/(\\d+)k(\\d+)b[m s].wav", "t:\\media\\audio\\adpcm", COPY_IF_NOT_EXIST },
		{NULL, NULL, 0}
	};
	// Load media
	hr = LoadMedia(media);

//"t:\\media\\audio\\ac3\\dd-broadway.wav"

    //
    // the following tests will Assert (by design) in debug builds
    // to turn these tests on (they are off by default) define
    // CODE_COVERAGE when this file is compiled
    //

	// Disable the busy wait warning 
	g_fDirectSoundDisableBusyWaitWarning = TRUE;

#ifdef CODE_COVERAGE

#endif // CODE_COVERAGE

	// Ensure that no previous DSound's are still running
	hr = ValidateResult(DirectSoundCreate(NULL, &pDirectSound, NULL));
	while(pDirectSound->Release());
	
    //
    // Test DsoundDRT in the mannor it was meant to be called
    //

    DirectSoundUseFullHRTF();

    //
    // Create the DirectSound object
    //
	
    hr = ValidateResult(DirectSoundCreate(NULL, &pDirectSound, NULL));

    //
    // Load the default effects image
    //

	DbgPrint("*****LoadEffectsImage(pDirectSound, \"t:\\media\\dspcode\\dsstdfx.bin\" )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "LoadEffectImage" );
	EXECUTE( LoadEffectsImage(pDirectSound, "t:\\media\\dspcode\\dsstdfx.bin" ) );

	DbgPrint("*****PlayOneShotBuffer( 0, WAVE_FORMAT_PCM, 48000, 16, 1 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "PlayOneShotBuffer PCM" );
	EXECUTE( PlayOneShotBuffer( 0, WAVE_FORMAT_PCM, 48000, 16, 1 ) );

	DbgPrint("*****PlayOneShotStream( 0, WAVE_FORMAT_PCM, 48000, 16, 1, 0x4000 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "PlayOneShotStream PCM" );
	EXECUTE( PlayOneShotStream( 0, WAVE_FORMAT_PCM, 48000, 16, 1, 0x4000 ) );


	DbgPrint("*****PlayOneShotStream( 0, WAVE_FORMAT_XBOX_ADPCM, 48000, 16, 1, 0x4000 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "PlayOneShotStream ADPCM" );
	EXECUTE( PlayOneShotStream( 0, WAVE_FORMAT_XBOX_ADPCM, 48000, 16, 1, 0x4000 ) );

	DbgPrint("*****PlayToneBuffer( 0, 8000, 8, 1, 400, 2000 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "PlayToneBuffer" );
	EXECUTE( PlayToneBuffer( 0, 8000, 8, 1, 400, 2000 ) );

	DbgPrint("*****Test3d( 0.0f, 0.0f, 5.0f, 5.0f, -72.0f, 200 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "Test3D" );
	EXECUTE( Test3d( 0.0f, 0.0f, 5.0f, 5.0f, -72.0f, 200 ) );

	DbgPrint("*****TestMultipleBuffers( WAVE_FORMAT_PCM )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestMultipleBuffers PCM" );
	EXECUTE( TestMultipleBuffers( WAVE_FORMAT_PCM ) );

	DbgPrint("*****TestMultipleBuffers( WAVE_FORMAT_XBOX_ADPCM )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestMultipleBuffers ADPCM" );
	EXECUTE( TestMultipleBuffers( WAVE_FORMAT_XBOX_ADPCM ) );

	DbgPrint("*****TestMultipleStreams( WAVE_FORMAT_PCM, 0x4000 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestMultipleStreams PCM" );
	EXECUTE( TestMultipleStreams( WAVE_FORMAT_PCM, 0x4000 ) );

	DbgPrint("*****TestMultipleStreams( WAVE_FORMAT_XBOX_ADPCM, 0x4000 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestMultipleStreams ADPCM" );
	EXECUTE( TestMultipleStreams( WAVE_FORMAT_XBOX_ADPCM, 0x4000 ) );

	DbgPrint("*****TestSequentialBuffers( WAVE_FORMAT_PCM )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestSequentialBuffers PCM" );
	EXECUTE( TestSequentialBuffers( WAVE_FORMAT_PCM ) );

	DbgPrint("*****TestSequentialBuffers( WAVE_FORMAT_XBOX_ADPCM )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestSequentialBuffers ADPCM" );
	EXECUTE( TestSequentialBuffers( WAVE_FORMAT_XBOX_ADPCM ) );

	DbgPrint("*****TestNotifies( WAVE_FORMAT_PCM, 22050, 16, 1, 20, FALSE )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestNotifies PCM" );
	EXECUTE( TestNotifies( WAVE_FORMAT_PCM, 22050, 16, 1, 20, FALSE ) );

	DbgPrint("*****TestNotifies( WAVE_FORMAT_XBOX_ADPCM, 22050, 16, 1, 20, FALSE )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestNotifies ADPCM" );
	EXECUTE( TestNotifies( WAVE_FORMAT_XBOX_ADPCM, 22050, 16, 1, 20, FALSE ) );

	DbgPrint("*****TestLoopRegion( WAVE_FORMAT_PCM, 48000, 16, 2, 0.5f, 1.0f )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestLoopRegion PCM" );
	EXECUTE( TestLoopRegion( WAVE_FORMAT_PCM, 48000, 16, 2, 0.5f, 1.0f ) );

	DbgPrint("*****TestLoopRegion( WAVE_FORMAT_XBOX_ADPCM, 48000, 16, 2, 0.5f, 1.0f )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestLoopRegion ADPCM" );
	EXECUTE( TestLoopRegion( WAVE_FORMAT_XBOX_ADPCM, 48000, 16, 2, 0.5f, 1.0f ) );

	DbgPrint("*****TestSubMix( TRUE, WAVE_FORMAT_PCM )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestSubMix PCM" );
	EXECUTE( TestSubMix( TRUE, WAVE_FORMAT_PCM ) );

	DbgPrint("*****TestSubMix( TRUE, WAVE_FORMAT_XBOX_ADPCM )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestSubMix ADPCM" );
	EXECUTE( TestSubMix( TRUE, WAVE_FORMAT_XBOX_ADPCM ) );

	DbgPrint("*****TestDLS( WAVE_FORMAT_PCM, 44100, 16, 1 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestDLS PCM" );
	EXECUTE( TestDLS( WAVE_FORMAT_PCM, 44100, 16, 1 ) );

	DbgPrint("*****TestDLS( WAVE_FORMAT_XBOX_ADPCM, 44100, 16, 1 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestDLS ADPCM" );
	EXECUTE( TestDLS( WAVE_FORMAT_XBOX_ADPCM, 44100, 16, 1 ) );

	DbgPrint("*****TestSurroundEncoder( WAVE_FORMAT_PCM, 44100, 16 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestSurroundEncoder PCM" );
	EXECUTE( TestSurroundEncoder( WAVE_FORMAT_PCM, 44100, 16 ) );

	DbgPrint("*****TestSurroundEncoder( WAVE_FORMAT_XBOX_ADPCM, 44100, 16 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestSurroundEncoder ADPCM" );
	EXECUTE( TestSurroundEncoder( WAVE_FORMAT_XBOX_ADPCM, 44100, 16 ) );

	DbgPrint("*****TestSgeSharing()*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestSGESharing" );
	EXECUTE( TestSgeSharing() );

	DbgPrint("*****TestAllVoices()*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestAllVoices" );
	EXECUTE( TestAllVoices() );

	DbgPrint("*****TestStop( WAVE_FORMAT_PCM, 44100, 16, 1 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestStop PCM" );
	EXECUTE( TestStop( WAVE_FORMAT_PCM, 44100, 16, 1 ) );

	DbgPrint("*****TestStop( WAVE_FORMAT_XBOX_ADPCM, 44100, 16, 1 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestStop ADPCM" );
	EXECUTE( TestStop( WAVE_FORMAT_XBOX_ADPCM, 44100, 16, 1 ) );

	DbgPrint("*****TestFrequency( DSBCAPS_CTRL3D, WAVE_FORMAT_PCM, 22050, 16, 1, 5000 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestFrequency PCM" );
	EXECUTE( TestFrequency( DSBCAPS_CTRL3D, WAVE_FORMAT_PCM, 22050, 16, 1, 5000 ) );

	DbgPrint("*****TestFrequency( DSBCAPS_CTRL3D, WAVE_FORMAT_XBOX_ADPCM, 22050, 16, 1, 5000 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestFrequency ADPCM" );
	EXECUTE( TestFrequency( DSBCAPS_CTRL3D, WAVE_FORMAT_XBOX_ADPCM, 22050, 16, 1, 5000 ) );

	DbgPrint("*****TestDiscontinuity( 0, WAVE_FORMAT_PCM, 22050, 16, 1, 0x4000, TRUE )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestDiscontinuity PCM" );
	EXECUTE( TestDiscontinuity( 0, WAVE_FORMAT_PCM, 22050, 16, 1, 0x4000, TRUE ) );

	DbgPrint("*****TestDiscontinuity( 0, WAVE_FORMAT_XBOX_ADPCM, 22050, 16, 1, 0x4000, TRUE )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestDiscontinuity ADPCM" );
	EXECUTE( TestDiscontinuity( 0, WAVE_FORMAT_XBOX_ADPCM, 22050, 16, 1, 0x4000, TRUE ) );

	DbgPrint("*****TestDeferredCommandRelease()*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestDeferredCommandRelease" );
	EXECUTE( TestDeferredCommandRelease() );

	DbgPrint("*****TestHeadroom()*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestHeadRoom" );
	EXECUTE( TestHeadroom() );

	DbgPrint("*****TestI3DL2Listener()*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestI3DL3Listener" );
	EXECUTE( TestI3DL2Listener() );

	DbgPrint("*****TestMultiChannel( 44100, 8, 1 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestMultipleChannel" );
	EXECUTE( TestMultiChannel( 44100, 8, 1 ) );

	DbgPrint("*****StressPlayStop( 2000 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "StressPlayStop" );
	EXECUTE( StressPlayStop( 2000 ) );

	DbgPrint("*****TestReleaseEnvelope()*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "ReleaseEnvelope" );
	EXECUTE( TestReleaseEnvelope() );

	DbgPrint("*****CompareVolume( 2.0f, 10 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "CompareVolume" );
	EXECUTE( CompareVolume( 2.0f, 10 ) );

	DbgPrint("*****TestNaturalStop( 1 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestNaturalStop" );
	EXECUTE( TestNaturalStop( 1 ) );

	DbgPrint("*****TestFlush()*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestFlush" );
	EXECUTE( TestFlush() );

	DbgPrint("*****TestFlushEx()*****\n");
	SETLOG( LogHandle, "robheit", "MCPX", "DsoundDRT", "TestFlushEx" );
	EXECUTE( TestFlushEx() );

	DbgPrint("*****TestPitch( 0, WAVE_FORMAT_PCM, 44100, 16, 1 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestPitch PCM" );
	EXECUTE( TestPitch( 0, WAVE_FORMAT_PCM, 44100, 16, 1 ) );

	DbgPrint("*****TestPitch( 0, WAVE_FORMAT_XBOX_ADPCM, 44100, 16, 1 )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestPitch ADPCM" );
	EXECUTE( TestPitch( 0, WAVE_FORMAT_XBOX_ADPCM, 44100, 16, 1 ) );

	DbgPrint("*****FindFrequencyRange(  )*****\n");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "FindFrequcnyRange" );
	EXECUTE( FindFrequencyRange(  ) );

	// Time Stamped Packets no longer supported
//	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "TestTimeStampedPackets" );
//	EXECUTE( TestTimeStampedPackets( 5000 ) );

    //
    // Release the DirectSound object before AC97 tests start
    //

    RELEASE( pDirectSound );

	//
	// Test the Ac97
	//

	DbgPrint("*****TestAc97Xmo( DSAC97_CHANNEL_ANALOG, DSAC97_MODE_PCM )*****");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "ac97 analog" );
    EXECUTE( TestAc97Xmo( DSAC97_CHANNEL_ANALOG, DSAC97_MODE_PCM ) );

	DbgPrint("*****TestAc97Xmo( DSAC97_CHANNEL_DIGITAL, DSAC97_MODE_PCM )*****");
	SETLOG( LogHandle, "danrose", "MCPX", "DsoundDRT", "ac97 digital" );
    EXECUTE( TestAc97Xmo( DSAC97_CHANNEL_DIGITAL, DSAC97_MODE_PCM ) );


}

/****************************************************************************
 *
 *  DsoundDRT_EndTest
 *
 *  Description:
 *      The Exit function for the test harness
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI DsoundDRT_EndTest( VOID )
{
}

/****************************************************************************
 *
 *  main
 *
 *  Description:
 *      the exe entry point
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#ifdef NOLOGGING
void __cdecl main( void )
{
    DsoundDRT_StartTest( NULL );
    DsoundDRT_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( DsoundDRT )
#pragma data_seg()

BEGIN_EXPORT_TABLE( DsoundDRT )
    EXPORT_TABLE_ENTRY( "StartTest", DsoundDRT_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DsoundDRT_EndTest )
END_EXPORT_TABLE( DsoundDRT )
