#ifdef XQUAKE_WMA

//-----------------------------------------------------------------------------
// File: wma.cpp
//
// Desc: Class for streaming wave file playback.
//
// Hist: 12.15.00 - New for December XDK release
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include "wma.h"


//-----------------------------------------------------------------------------
// Debugging help
//-----------------------------------------------------------------------------
#define MY_ASSERT(x) if( !(x) ) _asm int 3;


CWmaFileStream *g_pWmaStream;



EXTERN_C void SND_InitWmaPlayback()
{
    HRESULT     hr;

    g_pWmaStream = new CWmaFileStream();
    MY_ASSERT(g_pWmaStream);

    hr = g_pWmaStream->Initialize("T:\\MEDIA\\AUDIO\\WMA\\STD\\test.wma");
    if( FAILED(hr) )
    {
        delete g_pWmaStream;
        g_pWmaStream = NULL;
    }
}

EXTERN_C void SND_WmaProcessStream()
{

    if( g_pWmaStream )
        g_pWmaStream->Process(NULL);

}


//-----------------------------------------------------------------------------
// Name: CWmaFileStream()
// Desc: Object constructor.
//-----------------------------------------------------------------------------
CWmaFileStream::CWmaFileStream()
{
    m_pSourceFilter    = NULL;
    m_pRenderFilter    = NULL;
    m_pvSourceBuffer   = NULL;
    
    for( DWORD i = 0; i < FILESTRM_PACKET_COUNT; i++ )
        m_adwPacketStatus[i] = XMEDIAPACKET_STATUS_SUCCESS;

    m_dwStartingDataOffset = 0;
    m_dwFileLength   = 0;
    m_dwFileProgress = 0;
}




//-----------------------------------------------------------------------------
// Name: ~CWmaFileStream()
// Desc: Object destructor.
//-----------------------------------------------------------------------------
CWmaFileStream::~CWmaFileStream()
{
    if( m_pSourceFilter )      m_pSourceFilter->Release();
    if( m_pRenderFilter )      m_pRenderFilter->Release();
    if( m_pvSourceBuffer )     delete m_pvSourceBuffer;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initializes the wave file streaming subsystem.
//-----------------------------------------------------------------------------
HRESULT CWmaFileStream::Initialize( const CHAR* strFileName )
{
    WAVEFORMATEX   wfxSourceFormat;
    DSSTREAMDESC   dssd;

    HRESULT        hr;
    
    // Create the source (wma file) filter
    hr = WmaCreateDecoder(strFileName, NULL, FALSE, 16384, FILESTRM_PACKET_COUNT,2, &wfxSourceFormat, &m_pSourceFilter );
    if( FAILED(hr) )
        return hr;

    // Create the render (DirectSoundStream) filter
    ZeroMemory( &dssd, sizeof(dssd) );

#ifdef SILVER
    dssd.dwSize               = sizeof(dssd);
#endif // SILVER
    dssd.dwMaxAttachedPackets = FILESTRM_PACKET_COUNT;
    dssd.lpwfxFormat          = &wfxSourceFormat;

#ifdef SILVER
    hr = DirectSoundCreateStream( 0, &dssd, &m_pRenderFilter, NULL );
#else // SILVER
	hr = DirectSoundCreateStream( &dssd, &m_pRenderFilter );
#endif // SILVER
    if( FAILED(hr) )
        return hr;

    XMEDIAINFO xmi;

    // We expect the source filter to be synchronous and read-only, the
    // transform filter to be synchronous and read/write and the render
    // filter to be asynchronous write-only.  Assert that all of this 
    // is true and check the packet sizes for compatibility.

    hr = m_pSourceFilter->GetInfo( &xmi );
    if( FAILED(hr) )
        return hr;
    MY_ASSERT( xmi.dwFlags & XMO_STREAMF_FIXED_SAMPLE_SIZE);

    MY_ASSERT( !xmi.dwMaxLookahead );
    MY_ASSERT( xmi.dwOutputSize );
    MY_ASSERT( !(FILESTRM_SOURCE_PACKET_BYTES % xmi.dwOutputSize) );

    hr = m_pRenderFilter->GetInfo( &xmi );
    if( FAILED(hr) )
        return hr;
    MY_ASSERT( xmi.dwFlags == (XMO_STREAMF_FIXED_SAMPLE_SIZE | XMO_STREAMF_INPUT_ASYNC) );
    MY_ASSERT( FILESTRM_SOURCE_PACKET_BYTES * FILESTRM_PACKET_COUNT >= xmi.dwMaxLookahead );
    MY_ASSERT( !(FILESTRM_SOURCE_PACKET_BYTES % xmi.dwInputSize) );
    MY_ASSERT( !xmi.dwOutputSize );

    // Allocate data buffers.  Since the source filter is synchronous, we only
    // have to allocate enough data to process a single packet.  The render
    // filter, however, is asynchronous, so we'll have to allocate enough
    // space to hold all the packets that could be submitted at any given time.

    m_pvSourceBuffer = new BYTE[FILESTRM_SOURCE_PACKET_BYTES*FILESTRM_PACKET_COUNT];
    if( NULL == m_pvSourceBuffer )
        return E_OUTOFMEMORY;

    // Get the total wave file size.  We'll use this to calculate how far
    // along in the file we are during processing.
    hr = m_pSourceFilter->GetLength( &m_dwFileLength );
    if( FAILED(hr) )
        return hr;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Process()
// Desc: Performs any work necessary to keep the stream playing.
//-----------------------------------------------------------------------------
HRESULT CWmaFileStream::Process( DWORD* pdwPercentCompleted )
{
    DWORD   dwPacketIndex;
    HRESULT hr;
    
    // Find a free packet.  If there's none free, we don't have anything
    // to do
    while( FindFreePacket( &dwPacketIndex ) )
    {
         // Read from the source filter
         hr = ProcessSource(dwPacketIndex);
         if( FAILED(hr) )
             return hr;
         
         // Send the data to the renderer
         hr = ProcessRenderer(dwPacketIndex);
         if( FAILED(hr) )
             return hr;
    }

    // Calculate the completion percentage based on the total amount of
    // data we've read from the source.
    if( pdwPercentCompleted )
        (*pdwPercentCompleted) = m_dwFileProgress * 100 / m_dwFileLength;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FindFreePacket()
// Desc: Finds a render packet available for processing.
//-----------------------------------------------------------------------------
BOOL CWmaFileStream::FindFreePacket( DWORD* pdwPacketIndex )
{
    for( DWORD dwPacketIndex = 0; dwPacketIndex < FILESTRM_PACKET_COUNT; dwPacketIndex++ )
    {
        if( XMEDIAPACKET_STATUS_PENDING != m_adwPacketStatus[dwPacketIndex] )
        {
            if( pdwPacketIndex )
                (*pdwPacketIndex) = dwPacketIndex;

            return TRUE;
        }
    }

    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: ProcessSource()
// Desc: Reads data from the source filter.
//-----------------------------------------------------------------------------
HRESULT CWmaFileStream::ProcessSource(DWORD dwPacketIndex)
{
    DWORD        dwTotalSourceUsed   = 0;
    DWORD        dwSourceUsed;
    XMEDIAPACKET xmp;
    HRESULT      hr;
    
    // We're going to read a full packet's worth of data into the source
    // buffer.  Since we're playing in an infinite loop, we'll just spin
    // until we've read enough data, even if that means wrapping around the
    // end of the file.

    ZeroMemory( &xmp, sizeof(xmp) );
    xmp.pvBuffer         = (BYTE*)m_pvSourceBuffer + (dwPacketIndex * FILESTRM_SOURCE_PACKET_BYTES);
    xmp.dwMaxSize        = FILESTRM_SOURCE_PACKET_BYTES;
    xmp.pdwCompletedSize = &dwSourceUsed;

    while( dwTotalSourceUsed < FILESTRM_SOURCE_PACKET_BYTES )
    {
        // Read from the source
        hr = m_pSourceFilter->Process(NULL, &xmp);
        if( FAILED(hr) )
            return hr;

        // Add the amount read to the total
        dwTotalSourceUsed += dwSourceUsed;

        // If we read less than the amount requested, it's because we hit
        // the end of the file.  Seek back to the start and keep going.
        if( dwSourceUsed < xmp.dwMaxSize )
        {
            xmp.pvBuffer  = (BYTE*)xmp.pvBuffer + dwSourceUsed;
            xmp.dwMaxSize = xmp.dwMaxSize - dwSourceUsed;
            
            hr = m_pSourceFilter->Flush();
            if( FAILED(hr) )
                return hr;
        };
    }

    //
    // Update the file progress. We are playing of a compressed file
    // and the data we get back are decompressed audio. So on average the file has progressed
    // 1 byte for every N bytes we get.. N depends on the compression ratio of the file
    // Query directly the file position to figure out where we are
    //

    m_pSourceFilter->Seek(0,FILE_CURRENT,&m_dwFileProgress);
    m_dwFileProgress %= m_dwFileLength;

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: ProcessRenderer()
// Desc: Sends data to the renderer.
//-----------------------------------------------------------------------------
HRESULT CWmaFileStream::ProcessRenderer( DWORD dwPacketIndex )
{
    XMEDIAPACKET xmp;
    HRESULT      hr;

    // There's a full packet's worth of data ready for us to send to the
    // renderer.  We want to track the status of this packet since the
    // render filter is asychronous and we need to know when the packet is
    // completed.
    ZeroMemory( &xmp, sizeof(xmp) );
    xmp.pvBuffer  = (BYTE*)m_pvSourceBuffer + (dwPacketIndex * FILESTRM_SOURCE_PACKET_BYTES);
    xmp.dwMaxSize = FILESTRM_SOURCE_PACKET_BYTES;
    xmp.pdwStatus = &m_adwPacketStatus[dwPacketIndex];

    hr = m_pRenderFilter->Process( &xmp, NULL );
    if( FAILED(hr) )
        return hr;

    return S_OK;
}

#endif