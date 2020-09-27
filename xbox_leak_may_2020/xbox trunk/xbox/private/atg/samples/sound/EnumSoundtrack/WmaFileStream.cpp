//-----------------------------------------------------------------------------
// File: WmaFileStream.cpp
//
// Desc: WMA file playback
//
// Hist: 02.16.01 - New for March XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "WmaFileStream.h"
#include <cassert>




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

// The WMA decoder will work properly in async mode, with all formats
// only when using a lookahead size of at least 64k. We are using it synchronous mode
// but for good measure we still pass 64k
const DWORD WMASTRM_LOOKAHEAD_SIZE = 4096 * 16;

// This value is hard-coded assuming a WMA file of stero, 16bit resolution.  If
// this Value can by dynamically set based on the WMA format, keeping in mind
// that WMA needs enough buffer for a minimum of 2048 samples worth of PCM data
const DWORD WMASTRM_SOURCE_PACKET_BYTES = (2048*2*2);





//-----------------------------------------------------------------------------
// Name: CWMAFileStream()
// Desc: Object constructor
//-----------------------------------------------------------------------------
CWMAFileStream::CWMAFileStream()
:
    m_pSourceFilter  ( NULL ),
    m_pRenderFilter  ( NULL ),
    m_pSourceBuffer  ( NULL ),
    m_adwPacketStatus()
    
{
    for( DWORD i = 0; i < WMASTRM_PACKET_COUNT; ++i )
        m_adwPacketStatus[i] = XMEDIAPACKET_STATUS_SUCCESS;
}




//-----------------------------------------------------------------------------
// Name: ~CWMAFileStream()
// Desc: Object destructor
//-----------------------------------------------------------------------------
CWMAFileStream::~CWMAFileStream()
{
    Stop();
}




//-----------------------------------------------------------------------------
// Name: Start()
// Desc: Initializes the wave file streaming subsystem for playback
//-----------------------------------------------------------------------------
HRESULT CWMAFileStream::Start( HANDLE hFile )
{
    // Create the source (wma file) filter
    WAVEFORMATEX wfxSourceFormat;
    HRESULT hr = WmaCreateDecoder( "", hFile, FALSE, WMASTRM_LOOKAHEAD_SIZE,
                                   WMASTRM_PACKET_COUNT,
                                   0,
                                   &wfxSourceFormat, &m_pSourceFilter );
    if( FAILED(hr) )
        return hr;

    // Create the render (DirectSoundStream) filter
    DSSTREAMDESC dssd;
    ZeroMemory( &dssd, sizeof(dssd) );
    dssd.dwMaxAttachedPackets = WMASTRM_PACKET_COUNT;
    dssd.lpwfxFormat          = &wfxSourceFormat;

    hr = DirectSoundCreateStream( &dssd, &m_pRenderFilter );
    if( FAILED(hr) )
        return hr;

    // Allocate data buffers.  Since the source filter is synchronous, we only
    // have to allocate enough data to process a single packet.  The render
    // filter, however, is asynchronous, so we'll have to allocate enough
    // space to hold all the packets that could be submitted at any given time.
    m_pSourceBuffer = new BYTE[ WMASTRM_SOURCE_PACKET_BYTES * WMASTRM_PACKET_COUNT ];
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Stop()
// Desc: Stop the WMA playback; does all cleanup
//-----------------------------------------------------------------------------
VOID CWMAFileStream::Stop()
{
    if( m_pSourceFilter != NULL )
    {
        m_pSourceFilter->Release();
        m_pSourceFilter = NULL;
    }
    if( m_pRenderFilter != NULL )
    {
        m_pRenderFilter->Release();
        m_pRenderFilter = NULL;
    }
    if( m_pSourceBuffer )
    {
        delete[] m_pSourceBuffer;
        m_pSourceBuffer = NULL;
    }

    for( DWORD i = 0; i < WMASTRM_PACKET_COUNT; ++i )
        m_adwPacketStatus[i] = XMEDIAPACKET_STATUS_SUCCESS;
}




//-----------------------------------------------------------------------------
// Name: IsStopped()
// Desc: TRUE if playback stopped
//-----------------------------------------------------------------------------
BOOL CWMAFileStream::IsStopped() const
{
    return( m_pSourceFilter == NULL );
}




//-----------------------------------------------------------------------------
// Name: Process()
// Desc: Performs any work necessary to keep the stream playing.
//-----------------------------------------------------------------------------
HRESULT CWMAFileStream::Process()
{
    if( IsStopped() )
        return S_OK;

    // Find a free packet.  If there's none free, we don't have anything to do
    DWORD dwPacketIndex;
    while( FindFreePacket( dwPacketIndex ) )
    {
         // Read from the source filter
         HRESULT hr = ProcessSource( dwPacketIndex );
         if( FAILED(hr) )
             return hr;
         
         // Send the data to the renderer
         hr = ProcessRenderer( dwPacketIndex );
         if( FAILED(hr) )
             return hr;
    }
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FindFreePacket()
// Desc: Finds a render packet available for processing
//-----------------------------------------------------------------------------
BOOL CWMAFileStream::FindFreePacket( DWORD& dwPacketIndex ) const
{
    for( DWORD i = 0; i < WMASTRM_PACKET_COUNT; ++i )
    {
        if( XMEDIAPACKET_STATUS_PENDING != m_adwPacketStatus[i] )
        {
            dwPacketIndex = i;
            return TRUE;
        }
    }
    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: ProcessSource()
// Desc: Reads data from the source filter
//-----------------------------------------------------------------------------
HRESULT CWMAFileStream::ProcessSource( DWORD dwPacketIndex ) const
{
    // We're going to read a full packet's worth of data into the source
    // buffer.  Since we're playing in an infinite loop, we'll just spin
    // until we've read enough data, even if that means wrapping around the
    // end of the file.
    DWORD dwSourceUsed;
    XMEDIAPACKET xmp;
    ZeroMemory( &xmp, sizeof(xmp) );
    xmp.pvBuffer         = m_pSourceBuffer + 
                           ( dwPacketIndex * WMASTRM_SOURCE_PACKET_BYTES );
    xmp.dwMaxSize        = WMASTRM_SOURCE_PACKET_BYTES;
    xmp.pdwCompletedSize = &dwSourceUsed;

    DWORD dwTotalSourceUsed = 0;
    while( dwTotalSourceUsed < WMASTRM_SOURCE_PACKET_BYTES )
    {
        // Read from the source
        HRESULT hr = m_pSourceFilter->Process(NULL, &xmp);
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

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ProcessRenderer()
// Desc: Sends data to the renderer.
//-----------------------------------------------------------------------------
HRESULT CWMAFileStream::ProcessRenderer( DWORD dwPacketIndex )
{
    // There's a full packet's worth of data ready for us to send to the
    // renderer.  We want to track the status of this packet since the
    // render filter is asychronous and we need to know when the packet is
    // completed.
    XMEDIAPACKET xmp;
    ZeroMemory( &xmp, sizeof(xmp) );
    xmp.pvBuffer  = m_pSourceBuffer + (dwPacketIndex * WMASTRM_SOURCE_PACKET_BYTES );
    xmp.dwMaxSize = WMASTRM_SOURCE_PACKET_BYTES;
    xmp.pdwStatus = &m_adwPacketStatus[ dwPacketIndex ];

    HRESULT hr = m_pRenderFilter->Process( &xmp, NULL );
    if( FAILED(hr) )
        return hr;

    return S_OK;
}
