//-----------------------------------------------------------------------------
// File: WMAStream.cpp
//
// Desc: Class for streaming wave file playback.
//
// Hist: 12.15.00 - New for December XDK release
//       3.15.01 - Updated for April XDK 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <xtl.h>
#include "WMAStream.h"




//-----------------------------------------------------------------------------
// Debugging help
//-----------------------------------------------------------------------------
#define MY_ASSERT(x) if( !(x) ) _asm int 3;

BOOL g_bDoDebugChecks = FALSE;




//-----------------------------------------------------------------------------
// Name: CWMAFileStream()
// Desc: Object constructor.
//-----------------------------------------------------------------------------
CWMAFileStream::CWMAFileStream()
{
    m_pSourceFilter    = NULL;
    m_pRenderFilter    = NULL;
    m_pvSourceBuffer   = NULL;
    m_hrOpenResult     = E_PENDING;
    
    for( DWORD i = 0; i < WMASTRM_PACKET_COUNT; i++ )
        m_adwPacketStatus[i] = XMEDIAPACKET_STATUS_SUCCESS;

    m_dwStartingDataOffset = 0;
    m_dwFileProgress = 0;

    memset(&m_WmaContentDescription,0,sizeof(m_WmaContentDescription));
	
}





//-----------------------------------------------------------------------------
// Name: ~CWMAFileStream()
// Desc: Object destructor.
//-----------------------------------------------------------------------------
CWMAFileStream::~CWMAFileStream()
{
    if( m_pSourceFilter )      m_pSourceFilter->Release();
    if( m_pRenderFilter )      m_pRenderFilter->Release();
    if( m_pvSourceBuffer )     delete[] m_pvSourceBuffer;

    if (m_WmaContentDescription.pTitle)
        delete [] m_WmaContentDescription.pTitle;

    if (m_WmaContentDescription.pDescription)
        delete [] m_WmaContentDescription.pDescription;

    if (m_WmaContentDescription.pAuthor)
        delete [] m_WmaContentDescription.pAuthor;

}




//-----------------------------------------------------------------------------
// Name: InitializeOnThread()
// Desc: Performs initialization that must be done on worker thread
//-----------------------------------------------------------------------------
HRESULT CWMAFileStream::InitializeOnThread()
{
    HRESULT        hr;

    // Create the source (wma file) filter
    hr = WmaCreateDecoderEx( m_strFilename,             // File name
                             NULL,                      // instead of handle
                             FALSE,                     // Synchronous
                             WMASTRM_LOOKAHEAD_SIZE,    // Lookahead size
                             WMASTRM_PACKET_COUNT,      // # of packets
                             3,                         // Yield rate
                             &m_wfxSourceFormat,        // Wave format
                             &m_pSourceFilter );        // XMO pointer
    if( FAILED(hr) )
    {
        m_hrOpenResult = E_FAIL;
        return hr;
    }

    // Now we're done with our copy of the filename
    delete[] m_strFilename;

    // get some info about the wma content
    hr = m_pSourceFilter->GetFileContentDescription(&m_WmaContentDescription);
    if( FAILED(hr) )
    {
        m_hrOpenResult = E_FAIL;
        return hr;
    }

    // Create the render (DirectSoundStream) filter
    DSSTREAMDESC   dssd;
    ZeroMemory( &dssd, sizeof(dssd) );
    dssd.dwMaxAttachedPackets = WMASTRM_PACKET_COUNT;
    dssd.lpwfxFormat          = &m_wfxSourceFormat;

    hr = DirectSoundCreateStream( &dssd, &m_pRenderFilter );
    if( FAILED(hr) )
    {
        m_hrOpenResult = E_FAIL;
        return hr;
    }

    if( g_bDoDebugChecks )
    {
        XMEDIAINFO xmi;

        // We expect the source filter to be synchronous and read-only, the
        // transform filter to be synchronous and read/write and the render
        // filter to be asynchronous write-only.  Assert that all of this 
        // is true and check the packet sizes for compatibility.

        hr = m_pSourceFilter->GetInfo( &xmi );
        if( FAILED(hr) )
        {
            m_hrOpenResult = E_FAIL;
            return hr;
        }
        MY_ASSERT( xmi.dwFlags & XMO_STREAMF_FIXED_SAMPLE_SIZE );

        MY_ASSERT( !xmi.dwMaxLookahead );
        MY_ASSERT( xmi.dwOutputSize );
        MY_ASSERT( !(WMASTRM_SOURCE_PACKET_BYTES % xmi.dwOutputSize) );

        hr = m_pRenderFilter->GetInfo( &xmi );
        if( FAILED(hr) )
        {
            m_hrOpenResult = E_FAIL;
            return hr;
        }
        MY_ASSERT( xmi.dwFlags == (XMO_STREAMF_FIXED_SAMPLE_SIZE | XMO_STREAMF_INPUT_ASYNC) );
        MY_ASSERT( WMASTRM_SOURCE_PACKET_BYTES * WMASTRM_PACKET_COUNT >= xmi.dwMaxLookahead );
        MY_ASSERT( !(WMASTRM_SOURCE_PACKET_BYTES % xmi.dwInputSize) );
        MY_ASSERT( !xmi.dwOutputSize );
    }

    // Get the total wave file duration.  We'll use this to calculate how far
    // along in the file we are during processing.
    hr = m_pSourceFilter->GetFileHeader( &m_WmaFileHeader );
    if( FAILED(hr) )
    {
        m_hrOpenResult = E_FAIL;
        return hr;
    }

    m_hrOpenResult = S_OK;
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initializes the wave file streaming subsystem.
//-----------------------------------------------------------------------------
HRESULT CWMAFileStream::Initialize( const CHAR* strFileName, DWORD * pdwPercentCompleted )
{
    // Make a copy of the filename
    m_strFilename = new CHAR[ strlen( strFileName ) + 1 ];
    strcpy( m_strFilename, strFileName );

    m_pdwPercentCompleted = pdwPercentCompleted;
    
    m_WmaContentDescription.wTitleLength = 256;
    m_WmaContentDescription.wAuthorLength = 256;
    m_WmaContentDescription.wDescriptionLength = 256;

    // allocate buffers to hold the wma info

    m_WmaContentDescription.pTitle = new WCHAR[m_WmaContentDescription.wTitleLength];
    m_WmaContentDescription.pAuthor = new WCHAR[m_WmaContentDescription.wAuthorLength];
    m_WmaContentDescription.pDescription = new WCHAR[m_WmaContentDescription.wDescriptionLength];

    if ((m_WmaContentDescription.pDescription == NULL) ||
        (m_WmaContentDescription.pAuthor == NULL) ||
        (m_WmaContentDescription.pTitle == NULL)){
        
        return E_OUTOFMEMORY;

    }

    // Allocate data buffers.  Since the source filter is synchronous, we only
    // have to allocate enough data to process a single packet.  The render
    // filter, however, is asynchronous, so we'll have to allocate enough
    // space to hold all the packets that could be submitted at any given time.
    m_pvSourceBuffer = new BYTE[ WMASTRM_SOURCE_PACKET_BYTES * WMASTRM_PACKET_COUNT ];
    if( NULL == m_pvSourceBuffer )
        return E_OUTOFMEMORY;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Process()
// Desc: Performs any work necessary to keep the stream playing.
//-----------------------------------------------------------------------------
HRESULT CWMAFileStream::Process()
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
    // data we've played back.
    // NOTE: Process() is actually being called on our worker thread,
    // not the main title thread, so there could potentially be concurrent
    // access to the value pointed to by m_pdwPercentCompleted.  However,
    // this thread only writes to the value, and the main title thread
    // only reads it.  So the worst that could happen is that the main
    // title thread reads this just before we write to it, which is fine.
    // If more complex cross-thread operations are being performed, 
    // synchronization primitives may be required.
    if( m_pdwPercentCompleted )
        (*m_pdwPercentCompleted) = m_dwFileProgress * 100 / m_WmaFileHeader.dwDuration;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FindFreePacket()
// Desc: Finds a render packet available for processing.
//-----------------------------------------------------------------------------
BOOL CWMAFileStream::FindFreePacket( DWORD* pdwPacketIndex )
{
    for( DWORD dwPacketIndex = 0; dwPacketIndex < WMASTRM_PACKET_COUNT; dwPacketIndex++ )
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
HRESULT CWMAFileStream::ProcessSource(DWORD dwPacketIndex)
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
    xmp.pvBuffer         = (BYTE*)m_pvSourceBuffer + (dwPacketIndex * WMASTRM_SOURCE_PACKET_BYTES);
    xmp.dwMaxSize        = WMASTRM_SOURCE_PACKET_BYTES;
    xmp.pdwCompletedSize = &dwSourceUsed;

    while( dwTotalSourceUsed < WMASTRM_SOURCE_PACKET_BYTES )
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
    // Update the file progress. We know how much data in bytes, we have just played back
    // using the audio format of the wma file, determine how many ms we just played
    //

    dwTotalSourceUsed = (dwTotalSourceUsed*1000)/m_wfxSourceFormat.nAvgBytesPerSec;
    m_dwFileProgress = (m_dwFileProgress+dwTotalSourceUsed)%m_WmaFileHeader.dwDuration;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ProcessRenderer()
// Desc: Sends data to the renderer.
//-----------------------------------------------------------------------------
HRESULT CWMAFileStream::ProcessRenderer( DWORD dwPacketIndex )
{
    XMEDIAPACKET xmp;
    HRESULT      hr;

    // There's a full packet's worth of data ready for us to send to the
    // renderer.  We want to track the status of this packet since the
    // render filter is asychronous and we need to know when the packet is
    // completed.
    ZeroMemory( &xmp, sizeof(xmp) );
    xmp.pvBuffer  = (BYTE*)m_pvSourceBuffer + (dwPacketIndex * WMASTRM_SOURCE_PACKET_BYTES );
    xmp.dwMaxSize = WMASTRM_SOURCE_PACKET_BYTES;
    xmp.pdwStatus = &m_adwPacketStatus[dwPacketIndex];

    hr = m_pRenderFilter->Process( &xmp, NULL );
    if( FAILED(hr) )
        return hr;

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Pause
// Desc: Pauses and resumes stream playback
//-----------------------------------------------------------------------------
VOID
CWMAFileStream::Pause( DWORD dwPause )
{
    m_pRenderFilter->Pause( dwPause );
}



//-----------------------------------------------------------------------------
// Name: WMAFileStreamThreadProc
// Desc: Thread proc for sound processing worker thread
//-----------------------------------------------------------------------------
DWORD WINAPI WMAFileStreamThreadProc( LPVOID lpParameter )
{
    CWMAFileStream * pThis = (CWMAFileStream *)lpParameter;

    if( FAILED( pThis->InitializeOnThread() ) )
        return 0;

    // To process approximately once per frame, we can sleep
    // for 1000 ms / 60 fps between calls to Process.
    //
    // Alternately, the minimum time between processing is
    // determined by how much data we're sending to the stream
    // at once:
    // 2048 * 16 samples per packet * 16 packets = 524288 samples
    // 524288 samples / 44100 samples per second = 11.8 seconds
    //
    // DWORD dwQuantum = 11000;
 
    DWORD dwQuantum = 1000 / 60;

    for( ; ; )
    {
        pThis->Process();
        Sleep( dwQuantum );
    }
}

