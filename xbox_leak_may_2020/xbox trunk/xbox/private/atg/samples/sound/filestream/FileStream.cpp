//-----------------------------------------------------------------------------
// File: FileStream.cpp
//
// Desc: Class for streaming wave file playback.
//
// Hist: 12.15.00 - New for December XDK release
//       05.07.01 - Updated for hardware ADPCM
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include "FileStream.h"




//-----------------------------------------------------------------------------
// Debugging help
//-----------------------------------------------------------------------------
#define MY_ASSERT(x) if( !(x) ) _asm int 3;

BOOL g_bDoDebugChecks = TRUE;




//-----------------------------------------------------------------------------
// Name: CWaveFileStream()
// Desc: Object constructor.
//-----------------------------------------------------------------------------
CWaveFileStream::CWaveFileStream()
{
    m_pSourceFilter    = NULL;
    m_pRenderFilter    = NULL;
    m_pvSourceBuffer   = NULL;
    
    for( DWORD i = 0; i < FILESTRM_PACKET_COUNT; i++ )
        m_adwPacketStatus[i] = XMEDIAPACKET_STATUS_SUCCESS;

    m_dwFileLength   = 0;
    m_dwFileProgress = 0;
}




//-----------------------------------------------------------------------------
// Name: ~CWaveFileStream()
// Desc: Object destructor.
//-----------------------------------------------------------------------------
CWaveFileStream::~CWaveFileStream()
{
    if( m_pSourceFilter )      m_pSourceFilter->Release();
    if( m_pRenderFilter )      m_pRenderFilter->Release();
    if( m_pvSourceBuffer )     XPhysicalFree( m_pvSourceBuffer );
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initializes the wave file streaming subsystem.
//-----------------------------------------------------------------------------
HRESULT CWaveFileStream::Initialize( const CHAR* strFileName, DWORD * pdwPercentCompleted )
{
    LPCWAVEFORMATEX pwfxSourceFormat;
    DSSTREAMDESC    dssd;
    HRESULT         hr;

    m_pdwPercentCompleted = pdwPercentCompleted;
    
    // Create the source (wave file) filter
    hr = XWaveFileCreateMediaObject( strFileName, &pwfxSourceFormat, &m_pSourceFilter );
    if( FAILED(hr) )
        return hr;

    // For this sample, we're expecting an Xbox ADPCM wav file, but
    // normal PCM would be just fine.
    MY_ASSERT( WAVE_FORMAT_XBOX_ADPCM == pwfxSourceFormat->wFormatTag );

    // Create the render (DirectSoundStream) filter
    ZeroMemory( &dssd, sizeof(dssd) );
    dssd.dwMaxAttachedPackets = FILESTRM_PACKET_COUNT;
    dssd.lpwfxFormat          = (LPWAVEFORMATEX)pwfxSourceFormat;

    hr = DirectSoundCreateStream( &dssd, &m_pRenderFilter );
    if( FAILED(hr) )
        return hr;

    if( g_bDoDebugChecks )
    {
        XMEDIAINFO xmi;

        // We expect the source filter to be synchronous and read-only and the 
        // render filter to be asynchronous write-only.  Assert that all of
        // this is true and check the packet sizes for compatibility.

        hr = m_pSourceFilter->GetInfo( &xmi );
        if( FAILED(hr) )
            return hr;
        MY_ASSERT( xmi.dwFlags == (XMO_STREAMF_FIXED_SAMPLE_SIZE) );
        MY_ASSERT( !xmi.dwMaxLookahead );
        MY_ASSERT( !xmi.dwInputSize );
        MY_ASSERT( !(FILESTRM_PACKET_BYTES % xmi.dwOutputSize) );

        hr = m_pRenderFilter->GetInfo( &xmi );
        if( FAILED(hr) )
            return hr;
        MY_ASSERT( xmi.dwFlags == (XMO_STREAMF_FIXED_SAMPLE_SIZE | XMO_STREAMF_INPUT_ASYNC) );
        MY_ASSERT( FILESTRM_PACKET_BYTES * FILESTRM_PACKET_COUNT >= xmi.dwMaxLookahead );
        MY_ASSERT( !(FILESTRM_PACKET_BYTES % xmi.dwInputSize) );
        MY_ASSERT( !xmi.dwOutputSize );
    }

    // Allocate data buffers.  The render filter, however, is asynchronous, 
    // so we'll have to allocate enough space to hold all the packets that 
    // could be submitted at any given time.
    m_pvSourceBuffer = XPhysicalAlloc( FILESTRM_PACKET_BYTES * FILESTRM_PACKET_COUNT,
                                       MAXULONG_PTR,
                                       0,
                                       PAGE_READWRITE | PAGE_NOCACHE );
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
HRESULT CWaveFileStream::Process()
{
    DWORD   dwPacketIndex;
    HRESULT hr;
    
    // Find a free packet.  If there's none free, we don't have anything
    // to do
    while( FindFreePacket( &dwPacketIndex ) )
    {
        // Read from the source filter
        hr = ProcessSource( dwPacketIndex );
        if( FAILED(hr) )
            return hr;
        
        // Send the data to the renderer
        hr = ProcessRenderer( dwPacketIndex );
        if( FAILED(hr) )
            return hr;
    }

    // Calculate the completion percentage based on the total amount of
    // data we've read from the source.
    if( m_pdwPercentCompleted )
        (*m_pdwPercentCompleted) = m_dwFileProgress * 100 / m_dwFileLength;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FindFreePacket()
// Desc: Finds a render packet available for processing.
//-----------------------------------------------------------------------------
BOOL CWaveFileStream::FindFreePacket( DWORD* pdwPacketIndex )
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
HRESULT CWaveFileStream::ProcessSource( DWORD dwPacketIndex )
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
    xmp.pvBuffer         = (BYTE *)m_pvSourceBuffer + dwPacketIndex * FILESTRM_PACKET_BYTES;
    xmp.dwMaxSize        = FILESTRM_PACKET_BYTES;
    xmp.pdwCompletedSize = &dwSourceUsed;

    while( dwTotalSourceUsed < FILESTRM_PACKET_BYTES )
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
            
            hr = m_pSourceFilter->Seek( 0, FILE_BEGIN, NULL );
            if( FAILED(hr) )
                return hr;
        };
    }

    // Update the file progress
    m_dwFileProgress += dwTotalSourceUsed;
    m_dwFileProgress %= m_dwFileLength;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ProcessRenderer()
// Desc: Sends data to the renderer.
//-----------------------------------------------------------------------------
HRESULT CWaveFileStream::ProcessRenderer( DWORD dwPacketIndex )
{
    XMEDIAPACKET xmp;
    HRESULT      hr;

    // There's a full packet's worth of data ready for us to send to the
    // renderer.  We want to track the status of this packet since the
    // render filter is asychronous and we need to know when the packet is
    // completed.
    ZeroMemory( &xmp, sizeof(xmp) );
    xmp.pvBuffer  = (BYTE*)m_pvSourceBuffer + (dwPacketIndex * FILESTRM_PACKET_BYTES);
    xmp.dwMaxSize = FILESTRM_PACKET_BYTES;
    xmp.pdwStatus = &m_adwPacketStatus[dwPacketIndex];

    hr = m_pRenderFilter->Process( &xmp, NULL );
    if( FAILED(hr) )
        return hr;

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Pause
// Desc: Pauses or resumes stream playback
//-----------------------------------------------------------------------------
VOID
CWaveFileStream::Pause( DWORD dwPause )
{
    m_pRenderFilter->Pause( dwPause );
}



//-----------------------------------------------------------------------------
// Name: WAVFileStreamThreadProc
// Desc: Thread proc for sound processing worker thread
//-----------------------------------------------------------------------------
DWORD WINAPI WAVFileStreamThreadProc( LPVOID lpParameter )
{
    /*
    // To process approximately once per frame, we can sleep
    // for 1000 ms / 60 fps between calls to Process.
    */
    DWORD dwQuantum = 1000 / 60;

    /*
    // Alternately, the minimum time between processing is
    // determined by how much data we're sending to the stream
    // at once:
    // 2048 * 16 samples per packet * 16 packets = 524288 samples
    // 524288 samples / 44100 samples per second = 11.8 seconds
    //
    // DWORD dwQuantum = 11000;
    */

    CWaveFileStream * pThis = (CWaveFileStream *)lpParameter;

    for( ; ; )
    {
        pThis->Process();
        Sleep( dwQuantum );
    }
}


