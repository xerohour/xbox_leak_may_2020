#include <stdlib.h>
#include <stdio.h>
#include "echop.h"
#include "echo.h"

//
// implements a simple synchronous XMO and in-place
//

HRESULT
XCreateEchoFxMediaObject(
    DWORD dwDelay,
    DWORD dwSampleSize,
    XMediaObject **ppMediaObject
    )
{   
    HRESULT hr;
    CEchoXMO *pXmo;

    //
    // create net xmo
    //

    pXmo = new CEchoXMO();
    ASSERT(pXmo);
    if (pXmo == NULL) {
        return E_OUTOFMEMORY;
    }

    hr = pXmo->Initialize(dwDelay,
                          dwSampleSize);


    *ppMediaObject = NULL;

    if (SUCCEEDED(hr)) {
        *ppMediaObject = pXmo;
        pXmo->AddRef();
    }

    return hr;

}

HRESULT CEchoXMO::Initialize(
    DWORD dwDelay,
    DWORD dwSampleSize
    )
{

    //
    // this simple XMO only supports 16 bit samples (2 bytes)
    // however arbitrary sample size can be supported if the Process routine
    // treats input/dst buffers as arrays with proper size element size
    //

    ASSERT(dwSampleSize == (16 >> 3));
    m_dwSampleSize = dwSampleSize;
    m_dwDelay = dwDelay;

    //
    // allocate a buffer used as the delay line. Caches the previous packet..
    //

    m_pPreviousPacket = new SHORT[dwDelay/dwSampleSize];
    ASSERT(m_pPreviousPacket);

    memset(m_pPreviousPacket,0,dwDelay/dwSampleSize);
    return S_OK;

}



HRESULT STDMETHODCALLTYPE CEchoXMO::GetInfo( 
    PXMEDIAINFO pInfo
    )
{
    ASSERT(pInfo);

    //
    // we are a synchronous, in-place XMO
    //

    pInfo->dwFlags = XMO_STREAMF_IN_PLACE | XMO_STREAMF_FIXED_SAMPLE_SIZE |
        XMO_STREAMF_WHOLE_SAMPLES;

    //
    // because we are synchronous and not in place the buffers they pass
    // must be large enough to implement the echo effect
    // For a synchronous XMO, maxlookahead is irrelevant
    //

    pInfo->dwInputSize = m_dwDelay;
    pInfo->dwOutputSize = m_dwDelay;

    return S_OK;

}

HRESULT STDMETHODCALLTYPE CEchoXMO::GetStatus( 
        /* [out] */ DWORD __RPC_FAR *pdwFlags
        )
{

    //
    // let the caller know if we can accept any more data
    //

    ASSERT(pdwFlags);
    *pdwFlags = XMO_STATUSF_ACCEPT_INPUT_DATA | XMO_STATUSF_ACCEPT_OUTPUT_DATA;
    return S_OK;

}


HRESULT STDMETHODCALLTYPE CEchoXMO::Process( 
    const XMEDIAPACKET __RPC_FAR *pSrcBuffer,
    const XMEDIAPACKET __RPC_FAR *pDstBuffer
    )
{
    DWORD delayOffset = m_dwDelay/m_dwSampleSize;
    PSHORT dstBuffer, srcBuffer;
    SHORT srcSample;
    LONG temp32;

    //
    // we need both a destination and source buffer to operate
    //

    ASSERT(pSrcBuffer);
    ASSERT(pDstBuffer);

    //
    // make sure we have enough data to implement the echo effect
    //

    ASSERT(pSrcBuffer->dwMaxSize >= m_dwDelay);
    ASSERT(pDstBuffer->dwMaxSize >= m_dwDelay);
    ASSERT(pSrcBuffer->pvBuffer);
    ASSERT(pDstBuffer->pvBuffer);

    dstBuffer = (PSHORT) pDstBuffer->pvBuffer;
    srcBuffer = (PSHORT) pSrcBuffer->pvBuffer;

    //
    // echo effect
    //

    for (ULONG i=0;i<pDstBuffer->dwMaxSize/m_dwSampleSize;i++) {

        srcSample = srcBuffer[i];

        //
        // assume 16 bit samples. See comment in Initialize function.
        // Below we clip to avoid noise
        // 

        temp32 = m_pPreviousPacket[i]/2;

        temp32 += (LONG)srcSample;

        if (temp32 > 32767) {
            temp32 = 32767;
        } else if (temp32 < -32767) {
            temp32 = -32767;
        }

        dstBuffer[i] = (SHORT)temp32;

        //
        // cache the current src packet for the next call to process
        //

        m_pPreviousPacket[i] = srcSample;
    }

    if (pDstBuffer->pdwCompletedSize) {
        *pDstBuffer->pdwCompletedSize = pSrcBuffer->dwMaxSize;
    }

    if (pDstBuffer->pdwStatus) {
        *pDstBuffer->pdwStatus = XMEDIAPACKET_STATUS_SUCCESS;
    }

    if (pSrcBuffer->pdwCompletedSize) {
        *pSrcBuffer->pdwCompletedSize = pSrcBuffer->dwMaxSize;
    }

    if (pSrcBuffer->pdwStatus) {
        *pSrcBuffer->pdwStatus = XMEDIAPACKET_STATUS_SUCCESS;
    }

    //
    // set any events
    //

    if (pSrcBuffer->hCompletionEvent) {
        SetEvent(pSrcBuffer->hCompletionEvent);
    }

    if (pSrcBuffer->hCompletionEvent) {
        SetEvent(pSrcBuffer->hCompletionEvent);
    }

    return S_OK;

}


