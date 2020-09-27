//-----------------------------------------------------------------------------
// Name: XMOSampleRateConverter.cpp
//
// Desc: Sample rate converter interface. This is an example of deriving the
//       base XMediaObject into a custom interface.
//
// Hist: 12.15.00 - New for December XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "XMOSampleRateConverter.h"




//-----------------------------------------------------------------------------
// Name: CSampleRateConverter()
// Desc: Object constructor.
//-----------------------------------------------------------------------------
CSampleRateConverter::CSampleRateConverter( DWORD dwChannelCount, DWORD dwSourceFrequency, 
                                            DWORD dwDestinationFrequency )
{
    // Initialize members
    m_dwChannelCount         = dwChannelCount;
    m_dwSourceFrequency      = dwSourceFrequency;
    m_dwDestinationFrequency = dwDestinationFrequency;
    m_dwIndex                = 0L;
    m_ulRefCount             = 1L;

    // Calculate the input and output alignment.  Because we may produce or
    // consume more than one sample at a time (i.e. up- or downsampling),  we
    // need to calculate worst-case alignment.
    if( m_dwSourceFrequency < m_dwDestinationFrequency )
    {
        m_dwSourceAlignment      = ( m_dwDestinationFrequency + m_dwSourceFrequency - 1 ) / m_dwSourceFrequency;
        m_dwDestinationAlignment = 1;
    }
    else if( m_dwSourceFrequency > m_dwDestinationFrequency )
    {
        m_dwSourceAlignment      = 1;
        m_dwDestinationAlignment = ( m_dwSourceFrequency + m_dwDestinationFrequency - 1 ) / m_dwDestinationFrequency;
    }
    else
    {
        m_dwSourceAlignment      = 1;
        m_dwDestinationAlignment = 1;
    }
}




//-----------------------------------------------------------------------------
// Name: AddRef()
// Desc: Increments the object's reference count.
//-----------------------------------------------------------------------------
ULONG CSampleRateConverter::AddRef()
{
    return ++m_ulRefCount;
}




//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Decrements the object's reference count.
//-----------------------------------------------------------------------------
ULONG CSampleRateConverter::Release()
{
    if( !--m_ulRefCount )
    {
        delete this;
        return 0;
    }

    return m_ulRefCount;
}




//-----------------------------------------------------------------------------
// Name: GetInfo()
// Desc: Gets information about the data the XMO supports.
//-----------------------------------------------------------------------------
HRESULT CSampleRateConverter::GetInfo( XMEDIAINFO* pXMI )
{
    // Flags are easy.  We only support complete samples, but other than
    // that, we don't do much.
    pXMI->dwFlags = XMO_STREAMF_WHOLE_SAMPLES | XMO_STREAMF_FIXED_SAMPLE_SIZE;

    // Our input alignment is the minumum number of samples we require to
    // produce a single output sample.  We store our alignement in terms
    // of samples, but we need to return it in terms of bytes.  We only
    // work with 16-bit data, so converting samples to bytes is as simple
    // as multiplying by the channel count times 2 (16-bit samples == 2 bytes 
    // per sample).
    pXMI->dwInputSize = m_dwSourceAlignment * m_dwChannelCount * 2;

    // Output alignment is maximum number of samples we will every produce
    // when given a single input sample.
    pXMI->dwOutputSize = m_dwDestinationAlignment * m_dwChannelCount * 2;

    // We're a synchronous XMO, so MaxLookahead doesn't apply to us.  
    // Normally, this would be our "low watermark" value; i.e. the minimum
    // amount of data that must be queued at any given time in order to
    // prevent starvation.
    pXMI->dwMaxLookahead = 0;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetStatus()
// Desc: Gets the current status of the XMO.
//-----------------------------------------------------------------------------
HRESULT CSampleRateConverter::GetStatus( DWORD* pdwStatus )
{
    // We're a synchronous XMO, so we're always ready for more data.
    (*pdwStatus) = XMO_STATUSF_ACCEPT_INPUT_DATA | XMO_STATUSF_ACCEPT_OUTPUT_DATA;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Process()
// Desc: Processes data.
//-----------------------------------------------------------------------------
HRESULT CSampleRateConverter::Process( const XMEDIAPACKET* pSourceXMP,
                                       const XMEDIAPACKET* pDestinationXMP )
{
    // Save pointers to the data buffers
    SHORT* pSourceBuffer      = (SHORT*)pSourceXMP->pvBuffer;
    SHORT* pDestinationBuffer = (SHORT*)pDestinationXMP->pvBuffer;
    
    // Convert packet sizes from bytes to samples
    DWORD dwSourceSampleCount      = pSourceXMP->dwMaxSize / m_dwChannelCount / 2;
    DWORD dwDestinationSampleCount = pDestinationXMP->dwMaxSize / m_dwChannelCount / 2;

    // Enter the main loop
    while( ( dwSourceSampleCount >= m_dwSourceAlignment ) && 
           ( dwDestinationSampleCount >= m_dwDestinationAlignment ) )
    {
        // Calculate the number of source samples we'll be using in this
        // iteration.  If we're upsampling, we may produce more than one
        // output sample.  If we're downsampling, we may require more than
        // one input sample.  Because we keep a running SRC index, we'll
        // only produce or consume as many samples as we really need in order
        // to keep the sampling rate accurate.
        DWORD dwWorkingSourceSampleCount      = 1;
        DWORD dwWorkingDestinationSampleCount = 1;

        if( m_dwSourceFrequency < m_dwDestinationFrequency )
        {
            m_dwIndex += m_dwDestinationFrequency;
        
            dwWorkingDestinationSampleCount = m_dwIndex / m_dwSourceFrequency;

            if( dwWorkingDestinationSampleCount > dwDestinationSampleCount )
                dwWorkingDestinationSampleCount = dwDestinationSampleCount;

            m_dwIndex -= dwWorkingDestinationSampleCount * m_dwSourceFrequency;
        }
        else if( m_dwSourceFrequency > m_dwDestinationFrequency )
        {
            m_dwIndex += m_dwSourceFrequency;
        
            dwWorkingSourceSampleCount = m_dwIndex / m_dwDestinationFrequency;

            if( dwWorkingSourceSampleCount > dwSourceSampleCount )
                dwWorkingSourceSampleCount = dwSourceSampleCount;

            m_dwIndex -= dwWorkingSourceSampleCount * m_dwDestinationFrequency;
        }

        // Copy samples. Because we only consume multiple source samples
        // during a downsample operation, the loop below only references
        // the working destination sample count. Well, that and the channel
        // count, but you knew that.
        for( DWORD i = 0; i < dwWorkingDestinationSampleCount; i++ )
        {
            for( DWORD j = 0; j < m_dwChannelCount; j++ )
            {
                pDestinationBuffer[(m_dwChannelCount*i) + j] = pSourceBuffer[j];
            }
        }

        // Increment pointers and decrement sizes
        pSourceBuffer            += dwWorkingSourceSampleCount * m_dwChannelCount;
        dwSourceSampleCount      -= dwWorkingSourceSampleCount;

        pDestinationBuffer       += dwWorkingDestinationSampleCount * m_dwChannelCount;
        dwDestinationSampleCount -= dwWorkingDestinationSampleCount;
    }

    // Update the output parameters of the packet descriptions.
    if( pSourceXMP->pdwCompletedSize )
        (*pSourceXMP->pdwCompletedSize) = (BYTE*)pSourceBuffer - (BYTE*)pSourceXMP->pvBuffer;

    if( pSourceXMP->pdwStatus )
        (*pSourceXMP->pdwStatus) = XMEDIAPACKET_STATUS_SUCCESS;

    if( pSourceXMP->hCompletionEvent )
        SetEvent( pSourceXMP->hCompletionEvent );

    if( pDestinationXMP->pdwCompletedSize )
        (*pDestinationXMP->pdwCompletedSize) = (BYTE*)pDestinationBuffer - (BYTE*)pDestinationXMP->pvBuffer;

    if( pDestinationXMP->pdwStatus )
        (*pDestinationXMP->pdwStatus) = XMEDIAPACKET_STATUS_SUCCESS;

    if( pDestinationXMP->hCompletionEvent )
        SetEvent( pDestinationXMP->hCompletionEvent );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Discontinuity()
// Desc: Signals a discontinuity in the stream.
//-----------------------------------------------------------------------------
HRESULT CSampleRateConverter::Discontinuity()
{
    // Discontinuity doesn't mean anything to a synchronous XMO.  If we were
    // asynchronous, it would signal that no more source data is available
    // and all remaining destination packets should be completed.
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Flush()
// Desc: Resets the stream to it's default state.
//-----------------------------------------------------------------------------
HRESULT CSampleRateConverter::Flush()
{
    // We'll just reset the sample rate converter index
    m_dwIndex = 0;

    return S_OK;
}



