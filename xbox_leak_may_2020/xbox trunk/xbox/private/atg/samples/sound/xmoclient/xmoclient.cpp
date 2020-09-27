//-----------------------------------------------------------------------------
// Name: XMOClient.cpp
//
// Desc: This sample show an XMO client...an example of using a custom
//       XMediaObject.
//
// Hist: 12.15.00 - New for December XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include "XMOSampleRateConverter.h"




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const DWORD PLAYBACK_FREQUENCY                 = 48000;
const DWORD PACKET_COUNT                       = 4;
const DWORD PACKET_SIZE                        = 0x4000;




//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
VOID*                   g_pSourceBuffer                       = NULL;
VOID*                   g_pDestinationBuffer                  = NULL;
LPXFILEMEDIAOBJECT      g_pSourceFilter                       = NULL;
LPXMEDIAOBJECT          g_pIntermediateFilter                 = NULL;
LPDIRECTSOUNDSTREAM     g_pRenderFilter                       = NULL;
HANDLE                  g_ahPacketEvents[PACKET_COUNT];




//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Creates a file-based XMediaObject. In this case, we our using a
//       custom XMO (the sample rate converter object) that loads a file-based
//       sound file and converts its sample rate.
//-----------------------------------------------------------------------------
HRESULT Create( CHAR* strFilename )
{
    const WAVEFORMATEX* pwfxFile;
    WAVEFORMATEX        wfxStream;
    DSSTREAMDESC        dssd;
    HRESULT             hr;

    // We'll use a hard-coded packet size, because... well... I'm lazy.
    // In a shipping application, you'd want to calculate the optimal packet
    // size based on alignment restrictions and the MaxLookahead of any
    // asynchronous filters.  I'm going to make up a constant value.  If
    // the SRC has to do a significant downsample before sending to the
    // renderer, it's possible the stream will starve.

    // Create the source (wave file) filter.  We'll assume the source file is
    // 16-bit PCM.  Other than that, we don't really care what the format is.
    hr = XWaveFileCreateMediaObject( strFilename, &pwfxFile, &g_pSourceFilter );
    if( FAILED(hr) )
        return hr;

    // Create the intermediate (SRC) filter
    g_pIntermediateFilter = new CSampleRateConverter( pwfxFile->nChannels, 
                                                      pwfxFile->nSamplesPerSec, 
                                                      PLAYBACK_FREQUENCY );
    if( NULL == g_pIntermediateFilter )
        return E_OUTOFMEMORY;

    // Create the render (DirectSound stream) filter
    ZeroMemory( &dssd, sizeof(dssd) );
    dssd.dwMaxAttachedPackets = PACKET_COUNT;
    dssd.lpwfxFormat          = &wfxStream;
    wfxStream.wFormatTag      = WAVE_FORMAT_PCM;
    wfxStream.nChannels       = pwfxFile->nChannels;
    wfxStream.nSamplesPerSec  = PLAYBACK_FREQUENCY;
    wfxStream.wBitsPerSample  = pwfxFile->wBitsPerSample;
    wfxStream.nBlockAlign     = wfxStream.nChannels * wfxStream.wBitsPerSample / 8;
    wfxStream.nAvgBytesPerSec = wfxStream.nSamplesPerSec * wfxStream.nBlockAlign;

    hr = DirectSoundCreateStream( &dssd, &g_pRenderFilter );
    if( FAILED(hr) )
        return hr;

    // Allocate the source data buffer.  Because the source filter is 
    // synchronous, we can just use a single buffer and packet description.  
    // Because the amount of data we read from the source may be a different 
    // size than the amount we send to the renderer, using a single packet 
    // size is really sub-optimal.  We may end up re-reading data from the 
    // source because the renderer couldn't accept it all.
    g_pSourceBuffer = new BYTE[PACKET_SIZE];
    if( NULL == g_pSourceBuffer )
        return E_OUTOFMEMORY;

    // Allocate a buffer large enough to store packet data for all the 
    // renderer's packets. Because it's asynchronous, we have to keep the data
    // around until it's done processing it.
    g_pDestinationBuffer = new BYTE[PACKET_SIZE*PACKET_COUNT];
    if( NULL == g_pDestinationBuffer )
        return E_OUTOFMEMORY;

    // Create render packet completion events
    for( DWORD dwPacketIndex = 0; dwPacketIndex < PACKET_COUNT; dwPacketIndex++ )
    {
        g_ahPacketEvents[dwPacketIndex] = CreateEvent( NULL, TRUE, TRUE, NULL );
        if( NULL == g_ahPacketEvents[dwPacketIndex] )
            return E_OUTOFMEMORY;
    }

    return S_OK;

}




//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT Cleanup()
{
    // Wait for all packets to be completed
    WaitForMultipleObjects( PACKET_COUNT, g_ahPacketEvents, TRUE, INFINITE );

    // Free events
    for( DWORD dwPacketIndex = 0; dwPacketIndex < PACKET_COUNT; dwPacketIndex++ )
    {
        if( g_ahPacketEvents[dwPacketIndex] )
            CloseHandle( g_ahPacketEvents[dwPacketIndex] );
    }
    
    // Free memory and objects
    if( g_pSourceBuffer )       delete g_pSourceBuffer;
    if( g_pDestinationBuffer )  delete g_pDestinationBuffer;
    if( g_pSourceFilter )       g_pSourceFilter->Release();
    if( g_pIntermediateFilter ) g_pIntermediateFilter->Release();
    if( g_pRenderFilter )       g_pRenderFilter->Release();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Run()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT Run()
{
    // Enter the main loop
    while( TRUE )
    {
        HRESULT hr;

        // Wait for a packet completion event to be signalled.  If this is our
        // first time through the loop, all the packets are free, so all the
        // events are signalled.
        DWORD dwPacketIndex = WaitForMultipleObjects( PACKET_COUNT, g_ahPacketEvents, 
                                                      FALSE, INFINITE ) - WAIT_OBJECT_0;
        
        // Read some data
        XMEDIAPACKET xmpSource;
        DWORD        dwSourceCompletedSize;
        ZeroMemory( &xmpSource, sizeof(xmpSource) );
        xmpSource.pvBuffer         = g_pSourceBuffer;
        xmpSource.dwMaxSize        = PACKET_SIZE;
        xmpSource.pdwCompletedSize = &dwSourceCompletedSize;
        
        hr = g_pSourceFilter->Process( NULL, &xmpSource );
        if( FAILED(hr) )
            return hr;

        // If there's no more data to read, go ahead and break out of the loop
        if( 0 == dwSourceCompletedSize )
            return S_OK;

        // Send it through the SRC.  Be sure to fix up the packet size to
        // correspond to the completed size from the source filter.
        xmpSource.dwMaxSize = dwSourceCompletedSize;

        XMEDIAPACKET xmpDestination;
        DWORD        dwDestinationCompletedSize;
        ZeroMemory( &xmpDestination, sizeof(xmpDestination) );
        xmpDestination.pvBuffer         = (BYTE*)g_pDestinationBuffer + (dwPacketIndex * PACKET_SIZE);
        xmpDestination.dwMaxSize        = PACKET_SIZE;
        xmpDestination.pdwCompletedSize = &dwDestinationCompletedSize;

        hr = g_pIntermediateFilter->Process( &xmpSource, &xmpDestination );
        if( FAILED(hr) )
            return hr;

        // It's possible that we read some data, but it's not enough to pass
        // through the intermediate filter.  If that's true, go ahead and
        // break out of the loop.
        if( 0 == dwDestinationCompletedSize )
            return S_OK;

        // Send the packet to the renderer.  Again, be sure to fix up the 
        // packet size.  We don't care how much data actually gets played,
        // so we'll reset pdwCompletedSize to NULL.  If we did care how
        // much was played (or how much was processed by any asynchronous
        // XMO), we'd need to be sure that the variable pointed to by
        // pdwCompletedSize was not 
        xmpDestination.dwMaxSize        = dwDestinationCompletedSize;
        xmpDestination.pdwCompletedSize = NULL;
        xmpDestination.hCompletionEvent = g_ahPacketEvents[dwPacketIndex];

        ResetEvent( xmpDestination.hCompletionEvent );

        hr = g_pRenderFilter->Process( &xmpDestination, NULL );
        if( FAILED(hr) )
            return hr;

        // If the intermediate XMO processed less data than we supplied to
        // it, seek backwards in the source XMO. I outlined why this is lame
        // above, but oh well...
        if( dwSourceCompletedSize < xmpSource.dwMaxSize )
        {
            hr = g_pSourceFilter->Seek( -(LONG)(xmpSource.dwMaxSize - dwSourceCompletedSize), 
                                        FILE_CURRENT, NULL );
            if( FAILED(hr) )
                return hr;
        }
    }
}




//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    if( SUCCEEDED( Create( "D:\\Media\\XMOClnt.wav" ) ) )
    {
        Run();
    }

    Cleanup();
}




