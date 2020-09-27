//-----------------------------------------------------------------------------
// File: Echo.cpp
//
// Desc: Implements an XMO filter to add an echo effect.
//
// Hist: 01.06.00 - New for February XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "echo.h"




//-----------------------------------------------------------------------------
// Name: XCreateEchoFxMediaObject()
// Desc: Implements a simple synchronous XMO and in-place
//-----------------------------------------------------------------------------
HRESULT XCreateEchoFxMediaObject( XMediaObject** ppMediaObject, DWORD dwDelay,
                                  DWORD dwSampleSize )
{   
    // Create echo filter XMO
    CEchoXMO* pXmo = new CEchoXMO();
    if( NULL == pXmo )
        return E_OUTOFMEMORY;

    if( FAILED( pXmo->Initialize( dwDelay, dwSampleSize ) ) )
    {
        delete pXmo;
        (*ppMediaObject) = NULL;
        return E_FAIL;
    }
    (*ppMediaObject) = pXmo;
    (*ppMediaObject)->AddRef();
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CEchoXMO::Initialize( DWORD dwDelay, DWORD dwSampleSize )
{
    // This simple XMO only supports 16 bit samples (2 bytes).
    // However arbitrary sample size can be supported if the Process routine
    // treats input/dst buffers as arrays with proper size element size.
    if( dwSampleSize != (16>>3) )
        return E_INVALIDARG;
    
    m_dwSampleSize = dwSampleSize;
    m_dwDelay      = dwDelay;

    // Allocate a buffer used as the delay line. Caches the previous packet..
    m_pPreviousPacket = new SHORT[dwDelay/dwSampleSize];
    if( NULL == m_pPreviousPacket )
        return E_OUTOFMEMORY;

    ZeroMemory( m_pPreviousPacket, dwDelay/dwSampleSize );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetInfo()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CEchoXMO::GetInfo( XMEDIAINFO* pInfo )
{
    if( NULL==pInfo )
        return E_INVALIDARG;

    // We are a synchronous, in-place XMO
    pInfo->dwFlags = XMO_STREAMF_IN_PLACE;

    // Because we are synchronous and not in place the buffers they pass
    // must be large enough to implement the echo effect. For a synchronous 
    // XMO, maxlookahead is irrelevant
    pInfo->dwInputSize  = m_dwDelay;
    pInfo->dwOutputSize = m_dwDelay;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetStatus()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CEchoXMO::GetStatus( DWORD* pdwFlags )
{
    // Let the caller know if we can accept any more data
    if( pdwFlags )
        (*pdwFlags) = XMO_STATUSF_ACCEPT_INPUT_DATA;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Process()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CEchoXMO::Process( const XMEDIAPACKET* pSrcBuffer,
                                             const XMEDIAPACKET* pDstBuffer )
{
    // We need both a destination and source buffer to operate
    if( NULL==pSrcBuffer || NULL==pDstBuffer )
        return E_INVALIDARG;
    if( NULL==pSrcBuffer->pvBuffer || NULL==pDstBuffer->pvBuffer )
        return E_INVALIDARG;

    // Make sure we have enough data to implement the echo effect
    if( ( pSrcBuffer->dwMaxSize < m_dwDelay ) || ( pDstBuffer->dwMaxSize < m_dwDelay ) )
        return E_INVALIDARG;

    SHORT* pDstBufferData = (SHORT*)pDstBuffer->pvBuffer;
    SHORT* pSrcBufferData = (SHORT*)pSrcBuffer->pvBuffer;
    DWORD  dwDelayOffset  = m_dwDelay/m_dwSampleSize;

    // Echo effect
    for( DWORD i=0; i<pDstBuffer->dwMaxSize/m_dwSampleSize; i++ ) 
    {
        SHORT sample = pSrcBufferData[i];

        // Assume 16 bit samples. See comment in Initialize function.
        LONG temp32 = m_pPreviousPacket[i]/2 + (LONG)sample;

        // Clip to avoid noise
        if( temp32 > +32767 )   temp32 = +32767;
        if( temp32 < -32767 )   temp32 = -32767;

        pDstBufferData[i] = (SHORT)temp32;

        // Cache the current src packet for the next call to process
        m_pPreviousPacket[i] = sample;
    }

    if( pDstBuffer->pdwCompletedSize )   *(pDstBuffer->pdwCompletedSize) = pSrcBuffer->dwMaxSize;
    if( pDstBuffer->pdwStatus )          *(pSrcBuffer->pdwStatus)        = XMEDIAPACKET_STATUS_SUCCESS;
    if( pSrcBuffer->pdwCompletedSize )   *(pSrcBuffer->pdwCompletedSize) = pSrcBuffer->dwMaxSize;
    if( pSrcBuffer->pdwStatus )          *(pSrcBuffer->pdwStatus)        = XMEDIAPACKET_STATUS_SUCCESS;

    return S_OK;

}


