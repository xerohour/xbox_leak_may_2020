/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       SRCXMO.cpp
 *  Content:    Sample Rate Converter implementation
 *
 ****************************************************************************/

#include "SRCXMO.h"

/****************************************************************************
 *
 *  CSrcMediaObject
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      DWORD [in]: channel count.
 *      DWORD [in]: source sampling rate.
 *      DWORD [in]: destination sampling rate.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

CSrcMediaObject::CSrcMediaObject
(
    DWORD                   dwChannelCount, 
    DWORD                   dwSourceFrequency, 
    DWORD                   dwDestinationFrequency
)
:   m_dwChannelCount(dwChannelCount),
    m_dwSourceFrequency(dwSourceFrequency),
    m_dwDestinationFrequency(dwDestinationFrequency),
    m_dwSrcIndex(0),
    m_ulRefCount(1)
{
    //
    // Calculate the input and output alignment.  Because we may produce or
    // consume more than one sample at a time (i.e. up- or downsampling), 
    // we need to calculate worst-case alignment.
    //

    if(m_dwSourceFrequency < m_dwDestinationFrequency)
    {
        m_dwSourceAlignment = (m_dwDestinationFrequency + m_dwSourceFrequency - 1) / m_dwSourceFrequency;
        m_dwDestinationAlignment = 1;
    }
    else if(m_dwSourceFrequency > m_dwDestinationFrequency)
    {
        m_dwSourceAlignment = 1;
        m_dwDestinationAlignment = (m_dwSourceFrequency + m_dwDestinationFrequency - 1) / m_dwDestinationFrequency;
    }
    else
    {
        m_dwSourceAlignment = 1;
        m_dwDestinationAlignment = 1;
    }
}


/****************************************************************************
 *
 *  AddRef
 *
 *  Description:
 *      Increments the object's reference count.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      ULONG: new object reference count.
 *
 ****************************************************************************/

ULONG
CSrcMediaObject::AddRef
(
    void
)
{
    return ++m_ulRefCount;
}


/****************************************************************************
 *
 *  Release
 *
 *  Description:
 *      Decrements the object's reference count.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      ULONG: new object reference count.
 *
 ****************************************************************************/

ULONG
CSrcMediaObject::Release
(
    void
)
{
    if(!--m_ulRefCount)
    {
        delete this;
        return 0;
    }

    return m_ulRefCount;
}


/****************************************************************************
 *
 *  GetInfo
 *
 *  Description:
 *      Gets information about the data the XMO supports.
 *
 *  Arguments:
 *      LPXMEDIAINFO [out]: information data.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

HRESULT
CSrcMediaObject::GetInfo
(
    LPXMEDIAINFO            pxmi
)
{
    DWORD                   dwSourceSampleCount;
    DWORD                   dwDestinationSampleCount;
    
    //
    // Flags are easy.  We only support complete samples, but other than
    // that, we don't do much.
    //
    
    pxmi->dwFlags = XMO_STREAMF_FIXED_SAMPLE_SIZE;

    //
    // Our input alignment is the minumum number of samples we require to
    // produce a single output sample.  We store our alignement in terms
    // of samples, but we need to return it in terms of bytes.  We only
    // work with 16-bit data, so converting samples to bytes is as simple
    // as multiplying by the channel count times 2 (16-bit samples == 2 bytes 
    // per sample).
    //

    pxmi->dwInputSize = m_dwSourceAlignment * m_dwChannelCount * 2;

    //
    // Output alignment is maximum number of samples we will every produce
    // when given a single input sample.
    //

    pxmi->dwOutputSize = m_dwDestinationAlignment * m_dwChannelCount * 2;

    //
    // We're a synchronous XMO, so MaxLookahead doesn't apply to us.  
    // Normally, this would be our "low watermark" value; i.e. the minimum
    // amount of data that must be queued at any given time in order to
    // prevent starvation.
    //

    pxmi->dwMaxLookahead = 0;

    return S_OK;
}


/****************************************************************************
 *
 *  GetStatus
 *
 *  Description:
 *      Gets the current status of the XMO.
 *
 *  Arguments:
 *      LPDWORD [out]: status.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

HRESULT
CSrcMediaObject::GetStatus
(
    LPDWORD                 pdwStatus
)
{
    //
    // We're a synchronous XMO, so we're always ready for more data.
    //
    
    *pdwStatus = XMO_STATUSF_ACCEPT_INPUT_DATA | XMO_STATUSF_ACCEPT_OUTPUT_DATA;

    return S_OK;
}


/****************************************************************************
 *
 *  Process
 *
 *  Description:
 *      Processes data.
 *
 *  Arguments:
 *      LPCXMEDIAPACKET [in]: input packet.
 *      LPCXMEDIAPACKET [in]: output packet.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

HRESULT
CSrcMediaObject::Process
(
    LPCXMEDIAPACKET         pxmpSource,
    LPCXMEDIAPACKET         pxmpDestination
)
{
    LPSHORT                 psSourceBuffer;
    LPSHORT                 psDestinationBuffer;
    DWORD                   dwSourceSampleCount;
    DWORD                   dwDestinationSampleCount;
    DWORD                   dwWorkingSourceSampleCount;
    DWORD                   dwWorkingDestinationSampleCount;
    DWORD                   i, j;
    
    //
    // Save pointers to the data buffers
    //

    psSourceBuffer = (LPSHORT)pxmpSource->pvBuffer;
    psDestinationBuffer = (LPSHORT)pxmpDestination->pvBuffer;
    
    //
    // Convert packet sizes from bytes to samples
    //

    dwSourceSampleCount = pxmpSource->dwMaxSize / m_dwChannelCount / 2;
    dwDestinationSampleCount = pxmpDestination->dwMaxSize / m_dwChannelCount / 2;

    //
    // Enter the main loop
    //

    while((dwSourceSampleCount >= m_dwSourceAlignment) && (dwDestinationSampleCount >= m_dwDestinationAlignment))
    {
        //
        // Calculate the number of source samples we'll be using in this
        // iteration.  If we're upsampling, we may produce more than one
        // output sample.  If we're downsampling, we may require more than
        // one input sample.  Because we keep a running SRC index, we'll
        // only produce or consume as many samples as we really need in order
        // to keep the sampling rate accurate.
        //

        dwWorkingSourceSampleCount = 1;
        dwWorkingDestinationSampleCount = 1;

        if(m_dwSourceFrequency < m_dwDestinationFrequency)
        {
            m_dwSrcIndex += m_dwDestinationFrequency;
        
            dwWorkingDestinationSampleCount = m_dwSrcIndex / m_dwSourceFrequency;

            if(dwWorkingDestinationSampleCount > dwDestinationSampleCount)
            {
                dwWorkingDestinationSampleCount = dwDestinationSampleCount;
            }

            m_dwSrcIndex -= dwWorkingDestinationSampleCount * m_dwSourceFrequency;
        }
        else if(m_dwSourceFrequency > m_dwDestinationFrequency)
        {
            m_dwSrcIndex += m_dwSourceFrequency;
        
            dwWorkingSourceSampleCount = m_dwSrcIndex / m_dwDestinationFrequency;

            if(dwWorkingSourceSampleCount > dwSourceSampleCount)
            {
                dwWorkingSourceSampleCount = dwSourceSampleCount;
            }

            m_dwSrcIndex -= dwWorkingSourceSampleCount * m_dwDestinationFrequency;
        }

        //
        // Copy samples.  Because we only consume multiple source samples
        // during a downsample operation, the loop below only references
        // the working destination sample count.  Well, that and the channel
        // count, but you knew that.
        //

        for(i = 0; i < dwWorkingDestinationSampleCount; i++)
        {
            for(j = 0; j < m_dwChannelCount; j++)
            {
                psDestinationBuffer[j + (m_dwChannelCount * i)] = psSourceBuffer[j];
            }
        }

        //
        // Increment pointers and decrement sizes
        //

        psSourceBuffer += dwWorkingSourceSampleCount * m_dwChannelCount;
        dwSourceSampleCount -= dwWorkingSourceSampleCount;

        psDestinationBuffer += dwWorkingDestinationSampleCount * m_dwChannelCount;
        dwDestinationSampleCount -= dwWorkingDestinationSampleCount;
    }

    //
    // Update the output parameters of the packet descriptions.  There's really 
    // nothing you can do to cause us to error (short of a page fault), so we
    // always return success.  We'll calculate the number of bytes processed by
    // just looking at the difference between the current and starting data
    // buffer pointers.
    //

    if(pxmpSource->pdwCompletedSize)
    {
        *pxmpSource->pdwCompletedSize = (LPBYTE)psSourceBuffer - (LPBYTE)pxmpSource->pvBuffer;
    }

    if(pxmpSource->pdwStatus)
    {
        *pxmpSource->pdwStatus = XMEDIAPACKET_STATUS_SUCCESS;
    }

    if(pxmpSource->hCompletionEvent)
    {
        SetEvent(pxmpSource->hCompletionEvent);
    }

    if(pxmpDestination->pdwCompletedSize)
    {
        *pxmpDestination->pdwCompletedSize = (LPBYTE)psDestinationBuffer - (LPBYTE)pxmpDestination->pvBuffer;
    }

    if(pxmpDestination->pdwStatus)
    {
        *pxmpDestination->pdwStatus = XMEDIAPACKET_STATUS_SUCCESS;
    }

    if(pxmpDestination->hCompletionEvent)
    {
        SetEvent(pxmpDestination->hCompletionEvent);
    }

    return S_OK;
}


/****************************************************************************
 *
 *  Discontinuity
 *
 *  Description:
 *      Signals a discontinuity in the stream.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

HRESULT
CSrcMediaObject::Discontinuity
(
    void
)
{
    //
    // Discontinuity doesn't mean anything to a synchronous XMO.  If we were
    // asynchronous, it would signal that no more source data is available
    // and all remaining destination packets should be completed.
    //

    return S_OK;
}


/****************************************************************************
 *
 *  Flush
 *
 *  Description:
 *      Resets the stream to it's default state.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

HRESULT
CSrcMediaObject::Flush
(
    void
)
{
    //
    // We'll just reset the SRC index
    //

    m_dwSrcIndex = 0;

    return S_OK;
}