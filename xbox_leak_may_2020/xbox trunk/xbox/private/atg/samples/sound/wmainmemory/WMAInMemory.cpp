//-----------------------------------------------------------------------------
// File: WMAInMemory.cpp
//
// Desc: Class for streaming wave file playback using in-memory WMA codec XMO.
//
// Hist: 3.08.01 - New for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include "WMAInMemory.h"

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
    
    for( DWORD i = 0; i < WMASTRM_PACKET_COUNT; i++ )
        m_adwPacketStatus[i] = XMEDIAPACKET_STATUS_SUCCESS;

    m_dwFileLength   = 0;
    m_dwFileProgress = 0;
    m_pFileBuffer = NULL;
    m_hFile = NULL;

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

    if( m_pFileBuffer )        delete m_pFileBuffer;
    if( m_hFile )              CloseHandle(m_hFile);
    
    
}





//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initializes the wave file streaming subsystem.
//-----------------------------------------------------------------------------
HRESULT CWMAFileStream::Initialize( const CHAR* strFileName )
{
    WAVEFORMATEX   wfxSourceFormat;
    DSSTREAMDESC   dssd;
    HRESULT        hr;
    
    //
    // before we create the in memory decoder, we must read the WMA file
    // and have it in memory. The WmaCreateInMemoryDecoder function
    // will start calling our callbakc immediately for data...
    //

    m_hFile = CreateFile(strFileName,
                         GENERIC_READ,
                         FILE_SHARE_READ,
                         NULL,
                         OPEN_EXISTING,
                         0,
                         NULL);


    if (m_hFile == INVALID_HANDLE_VALUE) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    //
    // Determine size of WMA file
    //

    m_dwFileLength = SetFilePointer(m_hFile, 0, NULL, FILE_END);
    if (m_dwFileLength == INVALID_SET_FILE_POINTER) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;

    }
    
    //
    // restore file pointer to beginning of file
    //

    SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);


    //
    // allocate the buffer for the wma file
    //

    m_pFileBuffer = new BYTE[m_dwFileLength];
    if (m_pFileBuffer == NULL) {

        return E_OUTOFMEMORY;
    }

    //
    // read the whole file in. We are doing this for simplicity.
    // You can instead read a little bit at time making sure you stay ahead
    // of the WMA request file offset, passed in the callback..
    // 

    DWORD dwBytesRead = 0;

    if (!ReadFile(m_hFile,
                  m_pFileBuffer,
                  m_dwFileLength,
                  &dwBytesRead,
                  NULL)) {


        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;

    }

    // Create the source (wma file) filter
    hr = WmaCreateInMemoryDecoder(WMAStreamCallback,
                                  this,
                                  0, // dont yield
                                  &wfxSourceFormat,
                                  &m_pSourceFilter);

    if( FAILED(hr) )
        return hr;

    // Create the render (DirectSoundStream) filter
    ZeroMemory( &dssd, sizeof(dssd) );
    dssd.dwMaxAttachedPackets = WMASTRM_PACKET_COUNT;
    dssd.lpwfxFormat          = &wfxSourceFormat;

    hr = DirectSoundCreateStream( &dssd, &m_pRenderFilter );
    if( FAILED(hr) )
        return hr;

    if( g_bDoDebugChecks )
    {
        XMEDIAINFO xmi;

        // We expect the source filter to be synchronous and read-only, the
        // transform filter to be synchronous and read/write and the render
        // filter to be asynchronous write-only.  Assert that all of this 
        // is true and check the packet sizes for compatibility.

        hr = m_pSourceFilter->GetInfo( &xmi );
        if( FAILED(hr) )
            return hr;
        MY_ASSERT( xmi.dwFlags & XMO_STREAMF_FIXED_SAMPLE_SIZE );

        MY_ASSERT( !xmi.dwMaxLookahead );
        MY_ASSERT( xmi.dwOutputSize );
        MY_ASSERT( !(WMASTRM_SOURCE_PACKET_BYTES % xmi.dwOutputSize) );

        hr = m_pRenderFilter->GetInfo( &xmi );
        if( FAILED(hr) )
            return hr;
        MY_ASSERT( xmi.dwFlags == (XMO_STREAMF_FIXED_SAMPLE_SIZE | XMO_STREAMF_INPUT_ASYNC) );
        MY_ASSERT( WMASTRM_SOURCE_PACKET_BYTES * WMASTRM_PACKET_COUNT >= xmi.dwMaxLookahead );
        MY_ASSERT( !(WMASTRM_SOURCE_PACKET_BYTES % xmi.dwInputSize) );
        MY_ASSERT( !xmi.dwOutputSize );
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
// Name: WMAStreamCallback()
// Desc: The WMA decoder calls this function to retrieve raw (compressed) file data.
//-----------------------------------------------------------------------------

DWORD CALLBACK WMAStreamCallback (
    LPVOID pContext,
    ULONG offset,
    ULONG num_bytes,
    LPVOID *ppData)
{

    CWMAFileStream *pThis = (CWMAFileStream *)pContext;

    *ppData = pThis->m_pFileBuffer + offset;

    //
    // update current file offset for our progress bar
    //

    pThis->m_dwFileProgress = offset;
    return num_bytes;


}


//-----------------------------------------------------------------------------
// Name: Process()
// Desc: Performs any work necessary to keep the stream playing.
//-----------------------------------------------------------------------------
HRESULT CWMAFileStream::Process( DWORD* pdwPercentCompleted )
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
