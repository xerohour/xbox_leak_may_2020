//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBUtil.h>
#include <xgraphics.h>

#include "wmvplay.h"

//
// BUGBUG REMoE GLOBALS
//

DWORD g_dwImageSize = 0;
DWORD dwFrameRate = 0;
DWORD dwPrevVideoTimeStamp=0;

DWORD dwDecodeRate = 0;
DWORD dwLastDecodeTimeStamp = 0;

#if DBG


    ULONG g_WmvDebugLevel = 1;

#endif

DWORD WINAPI WmvThreadProc(
    LPVOID pParameter
    );

CFilterGraph *g_pThis;

//-----------------------------------------------------------------------------
// Name: Initialize
//
// Desc: Creates a streaming graph between 3 XMOs: a source, a in-between
//       (codec in this case) and a target. We only support one intermediate
//       xmo, but you could actually have multiple ones that manipulate the
//       same stream passing packets between them
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::Initialize()
{
    HRESULT        hr;
    DWORD          dwMinSize;
    XMEDIAINFO     xMediaInfo;
    DSSTREAMDESC   dssd;

    m_dwFrameCount = 0;

    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    //
    // init wmv decoder
    //

    InitWmv();

    m_dwBytesRead = 0;
    m_dwPacketSize     = PACKET_SIZE;
    m_dwMaxBufferCount = PACKET_CNT;

    PWAVEFORMATEX format=&m_wfxAudioFormat;

    format->wFormatTag          = WAVE_FORMAT_PCM;
    format->nSamplesPerSec      = m_AudStrm.u32SamplesPerSec;
    format->nChannels           = m_AudStrm.u16Channels;
    format->wBitsPerSample      = (WORD) m_AudStrm.u16BitsPerSample;
    format->nBlockAlign         = (16 * format->nChannels / 8);
    format->nAvgBytesPerSec     = format->nSamplesPerSec * format->nBlockAlign;
    format->cbSize              = 0;

    // Now create the target XMO, the end part of the streaming chain
    ZeroMemory( &dssd, sizeof(dssd) );
    dssd.dwSize               = sizeof(dssd);
    dssd.dwFlags              = 0;
    dssd.dwMaxAttachedPackets = PACKET_CNT;
    dssd.lpwfxFormat          = &m_wfxAudioFormat;

    hr = DirectSoundCreateStream( DSDEVID_MCPX, 
                                  &dssd, 
                                  (LPDIRECTSOUNDSTREAM*)&m_pTargetXMO,
                                  NULL );
    if( FAILED(hr) ) 
    {          
        DBG_ERROR( ("XmoFilter: DirectSoundCreateXMOStream failed with %x", hr) );
        DBG_ERROR( ("Init: Could not open a target audio XMO\n") );
        return hr;
    }

    //
    // At this point we should have all the XMOs.
    // Figure out the minimum packet size we need to pass media buffers
    // in between XMOs. All we do is verifying that our size (PACKET_SIZE)
    // is both larger than the min of all XMOs and its length aligned.
    // As a last check we make sure its also larger than the maxLookahead
    //
    // At the end of all this, m_dwMinPacketSize is the minimum packet size
    // for this filter graph.
    //
    ZeroMemory( &xMediaInfo, sizeof( XMEDIAINFO ) );

    if( m_pTargetXMO )
    {
        m_pTargetXMO->GetInfo( &xMediaInfo );
        m_dwMinPacketSize = max( xMediaInfo.dwOutputSize, m_dwMinPacketSize );

        XMOFILTER_VERIFY_PACKET_SIZE( xMediaInfo, m_dwPacketSize );
    }

    ASSERT( m_pTargetXMO );

    // Initialize the linked lists
    m_SourcePendingList.Initialize();
    m_TargetPendingList.Initialize();

    //
    // Allocate one large buffer for incoming/outgoing data. The buffer is
    // going to be divided among our media context packets, which will be 
    // used to stream data between XMOs
    //
    dwMinSize = MAXBUFSIZE;
    m_pTransferBuffer = new BYTE[dwMinSize];
    ZeroMemory( m_pTransferBuffer, dwMinSize );

    //
    // Allocate a context pool. A context is associated with a packet buffer
    // and used to track the pending packets attached to the source or target
    // XMO.
    dwMinSize = ( PACKET_CNT * sizeof(MEDIA_PACKET_CTX) );
    m_pPacketContextPool = new BYTE[dwMinSize];
    ZeroMemory( m_pPacketContextPool, dwMinSize );

    //
    // We attach all available packet buffers in the beginning to prime the
    // graph: As each packet is completed by the source XMO, the status is
    // udpated in its packet context and we pass the packet to the next XMO.
    //
    for( DWORD i = 0; i < PACKET_CNT; i++ ) 
    {
        MEDIA_PACKET_CTX* pCtx = &((MEDIA_PACKET_CTX*)m_pPacketContextPool)[i];

        ZeroMemory( pCtx, sizeof(MEDIA_PACKET_CTX) );
        pCtx->dwCompletedSize = m_dwPacketSize;
        pCtx->pBuffer         = &m_pTransferBuffer[i*PACKET_SIZE];

        AttachPacketToSourceXMO( pCtx );
    }

    //
    // create a thread for wmv decode
    //

    DWORD dwThreadId;
    m_hThread = CreateThread(NULL,
                             16384,
                             WmvThreadProc,
                             this,
                             0,
                             &dwThreadId);



    return S_OK;
}


HRESULT CFilterGraph::InitWmv()
{

    WORD wBitsPerPixel =24+8;
    m_hWmfDecoder = WMFDecodeCreate (this);
    WMPERR wmerr;

    g_pThis = this;

    //
    // open the file
    //

    m_hWmvFile = CreateFile("D:\\MEDIA\\test.asf",
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        NULL);

    if(m_hWmvFile == INVALID_HANDLE_VALUE) {

        return E_FAIL;

    }

    wmerr = WMFDecodeInit (m_hWmfDecoder, FOURCC_BI_RGB, wBitsPerPixel);
    if (wmerr != WMPERR_OK)
    {
        DBG_ERROR(("Decode Init Failed\n"));
        return wmerr;
    }   


    wmerr = WMFDecodeGetStreamInfo (m_hWmfDecoder, &m_AudStrm, &m_VidStrm);

    if (m_VidStrm.bHasValidVideo) {

        //
        // initialize graphics
        //
    
        InitGraphics();

        g_dwImageSize = m_VidStrm.u32Height * ((m_VidStrm.u32Width * wBitsPerPixel/8 +3)/4)*4;
        //m_pVidBuf = (UCHAR*) LocalAlloc(LPTR, m_VidStrm.u32Height * ((m_VidStrm.u32Width * wBitsPerPixel/8 +3)/4)*4);
        if (m_pVidBuf == NULL)
        {
            DBG_ERROR(("Can't allocate video buffer \n"));
            return 1;
        }
    }

    if(WMPERR_OK != wmerr)
    {
       DBG_ERROR(("Get Info Failed \n"));
       return E_FAIL;
    }

    wmerr = WMFDecodeSeek (m_hWmfDecoder, 0);
    if (WMPERR_OK != wmerr)
        return E_FAIL;

    return S_OK;



}

DWORD WINAPI WmvThreadProc(
    LPVOID pParameter
    )
{
    CFilterGraph *pThis = (CFilterGraph *) pParameter;
    pThis->ThreadProc();

    return 0;
}

VOID CFilterGraph::ThreadProc()
{


    DBG_INFO(("Calling decode\n"));

    while (1) {

        // Instruct routine to poll the filter graph so completed packets can be
        // moved through the streaming chain.

        ASSERT(!FAILED( g_pThis->TransferData() ) );
        ASSERT(WMFDecode (m_hWmfDecoder)==WMPERR_OK);

    }

}

WMPERR WMFCBDecodeGetData (HWMFDECODER hDecoder, PVOID pContext,char* pbuffer, U32_WMF cbSize, U64_WMF cbOffset, U32_WMF *cbRead)
{
    *cbRead=0;

    if (g_pThis->m_hWmvFile != NULL)
    {
        SetFilePointer(g_pThis->m_hWmvFile, (ULONG)cbOffset, NULL, FILE_BEGIN);
        if (!ReadFile(g_pThis->m_hWmvFile, pbuffer, cbSize, cbRead,NULL )) {

            return WMPERR_FAIL;

        }

        if (*cbRead != cbSize) {
            _asm int 3;
        }

    }

    return WMPERR_OK;
}

WMPERR WMFCBOutputDecodedAudio (HWMFDECODER hDecoder,PVOID pContext,const U8_WMF* pbAudio, U32_WMF cbAudioSize, U32_WMF cbSamples, U32_WMF cbTotAudioOut, U32_WMF dwTimeStamp)
{
    I32_WMF cbWritten =0;
    I32_WMF hr;

    if ((cbAudioSize >0) && (pbAudio != NULL))
    {

        MEDIA_PACKET_CTX* pSrcCtx = NULL;

        ASSERT(!FAILED( g_pThis->TransferData() ) );

        if( !g_pThis->m_SourcePendingList.IsListEmpty() ) 
        {
            pSrcCtx = (MEDIA_PACKET_CTX*)g_pThis->m_SourcePendingList.pNext; // Head

            while ((LINKED_LIST*)pSrcCtx != &g_pThis->m_SourcePendingList) {

                if (pSrcCtx->dwStatus == XMEDIAPACKET_STATUS_PENDING) {

                    break;

                }

                pSrcCtx = (MEDIA_PACKET_CTX*)pSrcCtx->ListEntry.pNext;

            }

            if (cbAudioSize == 0) {
                _asm int 3;
            }

            if (pSrcCtx->dwStatus == XMEDIAPACKET_STATUS_PENDING) {
                
                ASSERT(cbAudioSize <= PACKET_SIZE);
                memcpy(pSrcCtx->pBuffer, pbAudio, cbAudioSize);
        
                // Rset the state of our context
                pSrcCtx->dwStatus        = XMEDIAPACKET_STATUS_SUCCESS;
                pSrcCtx->dwCompletedSize = cbAudioSize;

            } else {

                DBG_ERROR((" Could not find empty audio packet\n"));

            }

        }
        DBG_INFO(("WMFCBOutputDecodedAudio: Got Audio Packet: Bytes %x, timestamp %x\n",
                    cbAudioSize, dwTimeStamp));


    }

    return WMPERR_OK;

}

WMPERR WMFCBGetVideoOutputBuffer (HWMFDECODER hDecoder,PVOID pContext,U8_WMF** ppucVideoOutoutBuffer)
{
    *ppucVideoOutoutBuffer = g_pThis->m_pVidBuf;    
    return WMPERR_OK;
}


WMPERR WMFCBOutputDecodedVideo (HWMFDECODER hDecoder,PVOID pContext,const U8_WMF *pbVideo, U32_WMF cbFrame, U32_WMF  biSizeImage, U32_WMF dwVideoTimeStamp)
{
    I32_WMF cbWritten =0;

    I32_WMF hr;
    static U32_WMF dwFrames =0;

    if (dwFrameRate > 0)
    {
        while ((dwVideoTimeStamp > dwPrevVideoTimeStamp)&&(dwVideoTimeStamp - dwPrevVideoTimeStamp)> (1000/dwFrameRate + 5) )
        {
        
            if (pbVideo !=NULL)
            {
                DBG_INFO(("Got Video Frame\n"));
            }
            dwPrevVideoTimeStamp +=(1000/dwFrameRate);
        }
    }
        
    dwFrameRate = 10000/(dwVideoTimeStamp - dwPrevVideoTimeStamp);
    dwFrameRate = (dwFrameRate +5)/10;

    dwDecodeRate = 10000/(GetTickCount() - dwLastDecodeTimeStamp);
    dwDecodeRate = (dwDecodeRate+5)/10;


    if (dwFrameRate) {

        DWORD dwFrameTime,dwDecodeInterval;
        DWORD dwSleepTime;

        dwFrameTime = 1000/dwFrameRate;
        dwDecodeInterval = (1000/dwDecodeRate) + 1;
        if (dwDecodeInterval < dwFrameTime) {

            dwSleepTime = dwFrameTime - dwDecodeInterval;
            Sleep(dwSleepTime);

            DBG_INFO(("Stalling decoder, sleeping for %x\n",
                     dwSleepTime));
        


        }

    }

    dwPrevVideoTimeStamp = dwVideoTimeStamp;
    dwLastDecodeTimeStamp = GetTickCount();
    
    return WMPERR_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup
// Desc: Clean up resources
//-----------------------------------------------------------------------------
HRESULT
CFilterGraph::Cleanup()
{
    // Cleanup: delete any XMO's
    if( m_pTargetXMO )       
        m_pTargetXMO->Release();


    if (m_hWmfDecoder) {

        WMFFlushBuffers(m_hWmfDecoder);
        WMFDecodeClose (&m_hWmfDecoder);

    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates, such as checking async xmo status, and
//       updating source file position for our progress bar
//-----------------------------------------------------------------------------
HRESULT
CFilterGraph::FrameMove()
{

    WMPERR wmerr;
    m_dwFrameCount++;

    //
    // call graphics routines
    //

    GraphicsFrameMove();
    Sleep((1000/60)/2);

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: TransferData
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::TransferData()
{
    HRESULT        hr=S_OK;
    MEDIA_PACKET_CTX* pSrcCtx = NULL;
    MEDIA_PACKET_CTX* pDstCtx = NULL;

    BOOL bSourcePacketCompleted = TRUE;
    BOOL bTargetPacketCompleted = TRUE;

    do {

        pSrcCtx = NULL;
        pDstCtx = NULL;

        //
        // Always deal with the oldest packet first.  Oldest packets are at the 
        // head, since we insert at the tail, and remove from head
        //
        if( !m_SourcePendingList.IsListEmpty() ) 
        {
            pSrcCtx = (MEDIA_PACKET_CTX*)m_SourcePendingList.pNext; // Head
        }
    
        if( !m_TargetPendingList.IsListEmpty() ) 
        {
            pDstCtx = (MEDIA_PACKET_CTX*)m_TargetPendingList.pNext; // Head
        }        
    
        // Do we have a completed source packet?
        if( pSrcCtx && pSrcCtx->dwStatus != XMEDIAPACKET_STATUS_PENDING ) 
        {
            bSourcePacketCompleted = TRUE;
            pSrcCtx->ListEntry.Remove();
    
            // Reset the state of our context
            pSrcCtx->dwStatus        = XMEDIAPACKET_STATUS_PENDING;
    
            // Pass media buffer to the target xmo.
            AttachPacketToTargetXMO( pSrcCtx );
        } else { 

            bSourcePacketCompleted = FALSE;

        }
    
        // Check if we have a completed packet from the target
        if( pDstCtx && pDstCtx->dwStatus != XMEDIAPACKET_STATUS_PENDING )
        {       
            DBG_SPAM(("Target XMO completed context %x\n",pDstCtx));    
            bTargetPacketCompleted = TRUE;

            // Remove the entry from the list
            pDstCtx->ListEntry.Remove();
    
            ASSERT(pDstCtx->dwCompletedSize != 0);
    
            // The target XMO is done with a buffer.
            pDstCtx->dwCompletedSize = PACKET_SIZE;
            AttachPacketToSourceXMO( pDstCtx );

        } else {

            bTargetPacketCompleted = FALSE;
        }


    } while (bSourcePacketCompleted || bTargetPacketCompleted);

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CFilterGraph xbApp;

    if( FAILED( xbApp.Create() ) )
        return;

    xbApp.Run();
}




//-----------------------------------------------------------------------------
// Name: AttachPacketToSourceXMO
//
// Desc: Helper function that prepares a packet context and attaches it to the
//       source XMO
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::AttachPacketToSourceXMO( MEDIA_PACKET_CTX* pCtx )
{
    HRESULT      hr;
    XMEDIAPACKET xmb;

    // Mark this packet as pending
    pCtx->dwStatus        = XMEDIAPACKET_STATUS_PENDING;

    // Set up our XMEDIAPACKET
    ZeroMemory( &xmb, sizeof(xmb) );
    xmb.pvBuffer         = pCtx->pBuffer;           // Point into buffer
    xmb.dwMaxSize        = pCtx->dwCompletedSize;   // Size of packet
    xmb.pdwCompletedSize = &pCtx->dwCompletedSize;  // Pointer to completed size
    xmb.pdwStatus        = &pCtx->dwStatus;         // Pointer to status

    // Add this context to our source list
    m_SourcePendingList.Add( &pCtx->ListEntry );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: AttachPacketToTargetXMO
//
// Desc: Helper function that prepares a packet context and attaches it to the
//       target XMO
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::AttachPacketToTargetXMO( MEDIA_PACKET_CTX* pCtx )
{
    HRESULT      hr;
    XMEDIAPACKET xmb;

    // Mark this packet as pending
    pCtx->dwStatus        = XMEDIAPACKET_STATUS_PENDING;

    // Set up our XMEDIAPACKET
    ZeroMemory( &xmb, sizeof(xmb) );
    xmb.pvBuffer         = pCtx->pBuffer;           // Point into buffer         
    xmb.dwMaxSize        = pCtx->dwCompletedSize;   // Size of packet
    xmb.pdwCompletedSize = &pCtx->dwCompletedSize;  // Pointer to completed size
    xmb.pdwStatus        = &pCtx->dwStatus;         // Pointer to status

    // Attach to the target XMO (first xmo in the chain)
    hr = m_pTargetXMO->Process( &xmb, NULL );
    if( FAILED(hr) ) 
    {
        DBG_ERROR(("InsertContext: Xmo %x, Process failed with hr = %x\n",hr));
    }

    // Add this context to our target list
    m_TargetPendingList.Add( &pCtx->ListEntry );

    return S_OK;
}



