//-----------------------------------------------------------------------------
// File: XMOFilter.cpp
//
// Desc: A sample to show:
//       (1) How to stream data between 3 xmos( 1 async sources, one sychronous 
//           in the middle, one async target.
//       (2) How to write a synchronous  effect or codec using XMOs and use it
//           in a graph.
//
//       Note: This sample requires a Hawk device to run.
//
// Hist: 01.06.00 - New for February XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include "Echo.h"
#include "xmofilter.h"
#include "xdpvoice.h"

// Debug macros
#define ASSERT(w) {if(w){}}
#define DBG_SPAM(_exp_)
#define DBG_INFO(_exp)
#define DBG_ERROR(_exp)
#define DBG_WARN(_exp)

//-----------------------------------------------------------------------------
// Name: AttachPacketToSourceXMO()
// Desc: Helper function that prepares a packet context and attaches it to the
//       source XMO
//-----------------------------------------------------------------------------
VOID CFilterGraph::AttachPacketToSourceXMO( MEDIA_PACKET_CTX* pCtx )
{
    HRESULT      hr;

    pCtx->dwStatus        = XMEDIAPACKET_STATUS_PENDING;

    XMEDIAPACKET xmb;
    ZeroMemory( &xmb, sizeof(xmb) );
    xmb.pvBuffer         = pCtx->pBuffer;            
    xmb.dwMaxSize        = pCtx->dwCompletedSize;
    xmb.hCompletionEvent = NULL;
    xmb.pdwCompletedSize = &pCtx->dwCompletedSize;
    xmb.pdwStatus        = &pCtx->dwStatus;

    // Attach to the source XMO (first xmo in the chain)
    hr = m_pSourceXMO->Process( NULL, &xmb );
    if( FAILED(hr) ) 
    {
        DBG_ERROR(("InsertContext: Xmo %x, Process failed with hr = %x\n",hr));
    }
    m_SourcePendingList.Add( &pCtx->ListEntry );
}




//-----------------------------------------------------------------------------
// Name: AttachPacketToTargetXMO()
// Desc: Helper function that prepares a packet context and attaches it to the
//       target XMO
//-----------------------------------------------------------------------------
VOID CFilterGraph::AttachPacketToTargetXMO( MEDIA_PACKET_CTX* pCtx )
{
    HRESULT      hr;

    pCtx->dwStatus        = XMEDIAPACKET_STATUS_PENDING;

    XMEDIAPACKET xmb;
    ZeroMemory( &xmb, sizeof(xmb) );
    xmb.pvBuffer         = pCtx->pBuffer;            
    xmb.dwMaxSize        = pCtx->dwCompletedSize;
    xmb.hCompletionEvent = NULL;
    xmb.pdwCompletedSize = &pCtx->dwCompletedSize;
    xmb.pdwStatus        = &pCtx->dwStatus;

    // Attach to the target XMO (first xmo in the chain)
    hr = m_pTargetXMO->Process( &xmb, NULL );
    if( FAILED(hr) ) 
    {
        DBG_ERROR(("InsertContext: Xmo %x, Process failed with hr = %x\n",hr));
    }
    m_TargetPendingList.Add( &pCtx->ListEntry );
}




//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Creates a streaming graph between 3 XMOs: a source, a in-between
//       (codec in this case) and a target. We only support one intermediate
//       xmo, but you could actually have multiple ones that manipulate the
//       same stream passing packets between them
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::Init(PWAVEFORMATEX pwfx,
                           XMediaObject *pSourceXmo,
                           XMediaObject *pIntermediateXmo,
                           XMediaObject *pTargetXmo)
                           
{
    HRESULT        hr = S_OK;
    DWORD          dwMinSize;

    XMEDIAINFO xMediaInfo;
    ZeroMemory( &xMediaInfo, sizeof( XMEDIAINFO ) );

    m_dwPacketSize     = PACKET_SIZE;
    m_dwMaxBufferCount = PACKET_CNT;

    if (pwfx) {

        memcpy(&m_wfxAudioFormat,pwfx,sizeof(WAVEFORMATEX));

    }


    if (pSourceXmo) {

        m_pSourceXMO = pSourceXmo;

    } else {

        //
        // default source is wmadecoder in async mode
        //

        hr = WmaCreateDecoder("D:\\MEDIA\\AUDIO\\test.wma",
                              NULL,
                              TRUE,
                              4096*16,
                              PACKET_CNT,
                              &m_wfxAudioFormat,
                              (XFileMediaObject **)&m_pSourceXMO );
        if( FAILED(hr) ) 
        {          
            DBG_ERROR( ("XmoFilter: WmaCreateDecoder failed with %x", hr) );
            return hr;
        }


    }

    if (pIntermediateXmo) {

        m_pIntermediateXMO = pIntermediateXmo;

    } else {

        // Now create a default in-between XMO (echo)
        hr = XCreateEchoFxMediaObject( &m_pIntermediateXMO,
                                       m_dwPacketSize,        // Length in bytes of delay line
                                       m_wfxAudioFormat.wBitsPerSample >> 3 ); // dwSampleSize in bytes
        if( FAILED(hr) ) 
        {          
            DBG_ERROR( ("XmoFilter: XcreateEchoFxMediaObject failed with %x", hr) );
            return hr;
        }

    }

    if (pTargetXmo) {

        m_pTargetXMO = pTargetXmo;

    } else {

        // create the default target XMO, the end part of the streaming chain
        DSSTREAMDESC dssd;
        ZeroMemory( &dssd, sizeof(dssd) );
        dssd.dwFlags              = 0;
        dssd.dwMaxAttachedPackets = PACKET_CNT;
        dssd.lpwfxFormat          = &m_wfxAudioFormat;
    
        hr = DirectSoundCreateStream( &dssd, (LPDIRECTSOUNDSTREAM*)&m_pTargetXMO );
        if( FAILED(hr) ) 
        {          
            DBG_ERROR( ("XmoFilter: DirectSoundCreateXMOStream failed with %x", hr) );
            DBG_ERROR( ("Init: Could not open a target audio XMO\n") );
            return hr;
        }

    }

#if DBG
    // At this point we should have all the XMOs.
    // Figure out the minimum packet size we need to pass media buffers
    // in between XMOs. All we do is verifying that our size (PACKET_SIZE)
    // is both larger than the min of all XMOs and its length aligned.
    // As a last check we make sure its also larger than the maxLookahead
    ZeroMemory( &xMediaInfo, sizeof( XMEDIAINFO ) );

    m_pSourceXMO->GetInfo( &xMediaInfo );
    m_dwMinPacketSize = xMediaInfo.dwOutputSize;

    XMOFILTER_VERIFY_PACKET_SIZE( xMediaInfo, m_dwPacketSize );

    m_pIntermediateXMO->GetInfo( &xMediaInfo );
    m_dwMinPacketSize = max( xMediaInfo.dwInputSize, m_dwMinPacketSize );

    XMOFILTER_VERIFY_PACKET_SIZE( xMediaInfo, m_dwPacketSize );

    m_pTargetXMO->GetInfo( &xMediaInfo );
    m_dwMinPacketSize = max( xMediaInfo.dwOutputSize, m_dwMinPacketSize );

    XMOFILTER_VERIFY_PACKET_SIZE( xMediaInfo, m_dwPacketSize );
#endif

    ASSERT( m_pTargetXMO );
    ASSERT( m_pSourceXMO );

    // Initialize the linked lists
    m_SourcePendingList.Initialize();
    m_TargetPendingList.Initialize();

    // Allocate one large buffer for incoming/outgoing data. The buffer is
    // going to be split in small packet segments that will be used to stream
    // data between XMOs
    dwMinSize = MAXBUFSIZE;
    m_pTransferBuffer = new BYTE[dwMinSize];
    ZeroMemory( m_pTransferBuffer, dwMinSize );

    // Allocate a context pool. A context is associated with a packet buffer
    // and used to track the pending packets attached to the source or target
    // XMO.
    dwMinSize = ( PACKET_CNT * sizeof(MEDIA_PACKET_CTX) ) + 10;
    m_pPacketContextPool = new BYTE[dwMinSize];
    memset( m_pPacketContextPool, 8, dwMinSize );

    // We attach all available packet buffers in the beginning to prime the
    // graph: As each packet is completed by the source XMO, the event is
    // signalled in its packet context and we pass the packet to the next XMO.
    for( DWORD i = 0; i < PACKET_CNT; i++ ) 
    {
        MEDIA_PACKET_CTX* pCtx = &((MEDIA_PACKET_CTX*)m_pPacketContextPool)[i];

        ZeroMemory( pCtx, sizeof(MEDIA_PACKET_CTX) );
        pCtx->dwCompletedSize = m_dwPacketSize;
        pCtx->dwStatus        = XMEDIAPACKET_STATUS_PENDING;
        pCtx->pBuffer         = &m_pTransferBuffer[i*PACKET_SIZE];

        AttachPacketToSourceXMO( pCtx );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: TransferData()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::TransferData()
{
    HRESULT        hr=S_OK;
    DWORD          index = 0;
    XMEDIAPACKET   xmb;
    MEDIA_PACKET_CTX* pSrcCtx = NULL;
    MEDIA_PACKET_CTX* pDstCtx = NULL;
    DWORD             dwSize  = 0;

    // Setup media buffer
    ZeroMemory( &xmb, sizeof(xmb) );

    // Wait for the oldest buffers submitted to the source and target xmo
    // oldest buffers are at the head, since we insert at the tail, remove from head
    if( !m_SourcePendingList.IsListEmpty() ) 
    {
        pSrcCtx = (MEDIA_PACKET_CTX*)m_SourcePendingList.pNext; // Head
    }

    if( !m_TargetPendingList.IsListEmpty() ) 
    {
        pDstCtx = (MEDIA_PACKET_CTX*)m_TargetPendingList.pNext; // Head
    }        

    if( pSrcCtx )
    {
        if( pSrcCtx->dwStatus != XMEDIAPACKET_STATUS_PENDING ) 
        {
            // A src buffer completed.
            // Remove this context from the Src pending list.
            // Send it to a codec, if present, then send it to the Target xmo
            pSrcCtx->ListEntry.Remove();

            // Before we pass it on, set the context packet to amount of data we received
            xmb.pvBuffer         = pSrcCtx->pBuffer;            
            xmb.dwMaxSize        = pSrcCtx->dwCompletedSize;        
            xmb.hCompletionEvent = NULL;

            DBG_SPAM(("Context %x received %x bytes, xmb %x\n",pSrcCtx, xmb.dwMaxSize, &xmb));
            ASSERT( xmb.dwMaxSize );

            pSrcCtx->dwCompletedSize = 0;
            pSrcCtx->dwStatus        = XMEDIAPACKET_STATUS_PENDING;

            dwSize = xmb.dwMaxSize;

            if( m_pIntermediateXMO ) 
            {
                // NOTE: its ok to pass a pointer to a stack variable since
                // middle XMO is sync!!! Never do this with async XMOs!!!
                // Note that we pass the same media packet description twice
                // THis is  because the particular intermediate effect XMO is in place
                // and it operates on a single buffer

                xmb.pdwCompletedSize = &dwSize;

                hr = m_pIntermediateXMO->Process( &xmb, &xmb );

                ASSERT(SUCCEEDED(hr));
                ASSERT(dwSize <=PACKET_SIZE);

                pSrcCtx->dwCompletedSize = dwSize;

            } 
            else // If intermediate xmo
            { 
                pSrcCtx->dwCompletedSize = m_dwPacketSize;
            }

            if (dwSize < PACKET_SIZE) {
        
                //
                // the source xmo produced less than asked. this signals end of file for wma xmo
                // flush it so it starts at the begininning again...
                // Pass at least a sample worth of data to dsound so it will not RIP
                //
        
                m_pSourceXMO->Flush();
                pSrcCtx->dwCompletedSize = max(4,dwSize);

            }

            // Pass media buffer to the target xmo.
            AttachPacketToTargetXMO( pSrcCtx );
        }


    }

    // Check if destination buffer xmo was also done..
    if( pDstCtx )
    {
        if( pDstCtx->dwStatus != XMEDIAPACKET_STATUS_PENDING )
        {       
            DBG_SPAM(("Target XMO completed context %x\n",pDstCtx));

            pDstCtx->ListEntry.Remove();

            ASSERT(pDstCtx->dwCompletedSize != 0);

            // The target XMO is done with a buffer.
            pDstCtx->dwCompletedSize = PACKET_SIZE;
            AttachPacketToSourceXMO( pDstCtx );
        } 
    }

    return S_OK;
}





