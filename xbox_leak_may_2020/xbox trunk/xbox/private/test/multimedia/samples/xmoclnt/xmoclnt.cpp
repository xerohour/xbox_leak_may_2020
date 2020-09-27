/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xmoclnt.cpp
 *  Content:    XMO client sample.
 *
 ****************************************************************************/

#include <xtl.h>
#include "SRCXMO.h"

/****************************************************************************
 *
 *  wWinMain
 *
 *  Description:
 *      Application entry point.
 *
 *  Arguments:
 *      HINSTANCE [in]: unused.
 *      HINSTANCE [in]: unused.
 *      LPWSTR [in]: unused.
 *      int [in]: unused.
 *
 *  Returns:  
 *      int: 0.
 *
 ****************************************************************************/

void __cdecl main()
{
    static const LPCSTR     pszFileName                         = "t:\\media\\xmoclnt.wav";
    static const DWORD      dwPlaybackFrequency                 = 48000;
    static const DWORD      dwPacketCount                       = 4;
    static const DWORD      dwPacketSize                        = 0x4000;
    LPXFILEMEDIAOBJECT      pSourceFilter                       = NULL;
    LPXMEDIAOBJECT          pIntermediateFilter                 = NULL;
    LPDIRECTSOUNDSTREAM     pRenderFilter                       = NULL;
    LPVOID                  pvSourceBuffer                      = NULL;
    LPVOID                  pvDestinationBuffer                 = NULL;
    HANDLE                  ahPacketEvents[dwPacketCount];
    XMEDIAPACKET            xmpSource;
    XMEDIAPACKET            xmpDestination;
    DWORD                   dwSourceCompletedSize;
    DWORD                   dwDestinationCompletedSize;
    DWORD                   dwPacketIndex;
    LPCWAVEFORMATEX         pwfxFile;
    WAVEFORMATEX            wfxStream;
    DSSTREAMDESC            dssd;
    HRESULT                 hr;

    //
    // We'll use a hard-coded packet size, because... well... I'm lazy.
    // In a shipping application, you'd want to calculate the optimal packet
    // size based on alignment restrictions and the MaxLookahead of any
    // asynchronous filters.  I'm going to make up a constant value.  If
    // the SRC has to do a significant downsample before sending to the
    // renderer, it's possible the stream will starve.
    //

    //
    // Create the source (wave file) filter.  We'll assume the source file
    // is 16-bit PCM.  Other than that, we don't really care what the format
    // is.
    //

    hr = XWaveFileCreateMediaObject(pszFileName, &pwfxFile, &pSourceFilter);

    //
    // Create the intermediate (SRC) filter
    //

    if(SUCCEEDED(hr))
    {
        if(!(pIntermediateFilter = new CSrcMediaObject(pwfxFile->nChannels, pwfxFile->nSamplesPerSec, dwPlaybackFrequency)))
        {
            hr = E_OUTOFMEMORY;
        }
    }

    //
    // Create the render (DirectSound stream) filter
    //

    if(SUCCEEDED(hr))
    {
        wfxStream.wFormatTag = WAVE_FORMAT_PCM;
        wfxStream.nChannels = pwfxFile->nChannels;
        wfxStream.nSamplesPerSec = dwPlaybackFrequency;
        wfxStream.wBitsPerSample = pwfxFile->wBitsPerSample;
        wfxStream.nBlockAlign = wfxStream.nChannels * wfxStream.wBitsPerSample / 8;
        wfxStream.nAvgBytesPerSec = wfxStream.nSamplesPerSec * wfxStream.nBlockAlign;
        
        ZeroMemory(&dssd, sizeof(dssd));

#ifdef SILVER
        dssd.dwSize = sizeof(dssd);
#endif // SILVER
        dssd.dwMaxAttachedPackets = dwPacketCount;
        dssd.lpwfxFormat = &wfxStream;

#ifdef SILVER
        hr = DirectSoundCreateStream(DSDEVID_MCPX, &dssd, &pRenderFilter, NULL);
#else // SILVER
		hr = DirectSoundCreateStream( &dssd, &pRenderFilter );
#endif // SILVER
    }

    //
    // Allocate the source data buffer.  Because the source filter is 
    // synchronous, we can just use a single buffer and packet description.  
    // Because the amount of data we read from the source may be a different 
    // size than the amount we send to the renderer, using a single packet 
    // size is really sub-optimal.  We may end up re-reading data from the 
    // source because the renderer couldn't accept it all.  Oh well.  Sample 
    // code isn't supposed to be great.
    //

    if(SUCCEEDED(hr))
    {
        if(!(pvSourceBuffer = LocalAlloc(LPTR, dwPacketSize)))
        {
            hr = E_OUTOFMEMORY;
        }
    }

    //
    // Allocate a buffer large enough to store packet data for all the 
    // renderer's packets.  Because he's asynchronous, we have to keep the
    // data around until he's done processing it.
    //

    if(SUCCEEDED(hr))
    {
        if(!(pvDestinationBuffer = LocalAlloc(LPTR, dwPacketSize * dwPacketCount)))
        {
            hr = E_OUTOFMEMORY;
        }
    }

    //
    // Create render packet completion events
    //

    if(SUCCEEDED(hr))
    {
        for(dwPacketIndex = 0; dwPacketIndex < dwPacketCount; dwPacketIndex++)
        {
            if(!(ahPacketEvents[dwPacketIndex] = CreateEvent(NULL, TRUE, TRUE, NULL)))
            {
                hr = E_OUTOFMEMORY;
                break;
            }
        }
    }

    //
    // Enter the main loop
    //

    while(SUCCEEDED(hr))
    {
        //
        // Wait for a packet completion event to be signalled.  If this is our
        // first time through the loop, all the packets are free, so all the
        // events are signalled.
        //

        dwPacketIndex = WaitForMultipleObjects(dwPacketCount, ahPacketEvents, FALSE, INFINITE) - WAIT_OBJECT_0;
        
        //
        // Read some data
        //

        ZeroMemory(&xmpSource, sizeof(xmpSource));
        
        xmpSource.pvBuffer = pvSourceBuffer;
        xmpSource.dwMaxSize = dwPacketSize;
        xmpSource.pdwCompletedSize = &dwSourceCompletedSize;
        
        hr = pSourceFilter->Process(NULL, &xmpSource);

        //
        // If there's no more data to read, go ahead and break out of the loop
        //

        if(SUCCEEDED(hr) && !dwSourceCompletedSize)
        {
            break;
        }

        //
        // Send it through the SRC.  Be sure to fix up the packet size to
        // correspond to the completed size from the source filter.
        //

        if(SUCCEEDED(hr))
        {
            xmpSource.dwMaxSize = dwSourceCompletedSize;

            ZeroMemory(&xmpDestination, sizeof(xmpDestination));

            xmpDestination.pvBuffer = (LPBYTE)pvDestinationBuffer + (dwPacketIndex * dwPacketSize);
            xmpDestination.dwMaxSize = dwPacketSize;
            xmpDestination.pdwCompletedSize = &dwDestinationCompletedSize;

            hr = pIntermediateFilter->Process(&xmpSource, &xmpDestination);
        }

        //
        // It's possible that we read some data, but it's not enough to pass
        // through the intermediate filter.  If that's true, go ahead and
        // break out of the loop.
        //
        
        if(SUCCEEDED(hr) && !dwDestinationCompletedSize)
        {
            break;
        }

        //
        // Send the packet to the renderer.  Again, be sure to fix up the 
        // packet size.  We don't care how much data actually gets played,
        // so we'll reset pdwCompletedSize to NULL.  If we did care how
        // much was played (or how much was processed by any asynchronous
        // XMO), we'd need to be sure that the variable pointed to by
        // pdwCompletedSize was not 
        //

        if(SUCCEEDED(hr))
        {
            xmpDestination.dwMaxSize = dwDestinationCompletedSize;
            xmpDestination.pdwCompletedSize = NULL;
            xmpDestination.hCompletionEvent = ahPacketEvents[dwPacketIndex];

            ResetEvent(xmpDestination.hCompletionEvent);

            hr = pRenderFilter->Process(&xmpDestination, NULL);
        }

        //
        // If the intermediate XMO processed less data than we supplied to
        // it, seek backwards in the source XMO.  I outlined why this is lame
        // above, but oh well...
        //

        if(SUCCEEDED(hr) && (dwSourceCompletedSize < xmpSource.dwMaxSize))
        {
            hr = pSourceFilter->Seek(-(LONG)(xmpSource.dwMaxSize - dwSourceCompletedSize), FILE_CURRENT, NULL);
        }
    }

    //
    // Wait for all packets to be completed
    //

    WaitForMultipleObjects(dwPacketCount, ahPacketEvents, TRUE, INFINITE);

    //
    // Free events
    //

    for(dwPacketIndex = 0; dwPacketIndex < dwPacketCount; dwPacketIndex++)
    {
        if(ahPacketEvents[dwPacketIndex])
        {
            CloseHandle(ahPacketEvents[dwPacketIndex]);
        }
    }
    
    //
    // Free memory
    //

    if(pvSourceBuffer)
    {
        LocalFree(pvSourceBuffer);
    }

    if(pvDestinationBuffer)
    {
        LocalFree(pvDestinationBuffer);
    }

    //
    // Free objects
    //

    if(pSourceFilter)
    {
        pSourceFilter->Release();
    }

    if(pIntermediateFilter)
    {
        pIntermediateFilter->Release();
    }

    if(pRenderFilter)
    {
        pRenderFilter->Release();
    }
}