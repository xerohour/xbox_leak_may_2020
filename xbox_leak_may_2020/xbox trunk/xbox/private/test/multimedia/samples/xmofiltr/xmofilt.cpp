#include "xmofilt.h"
#include "echo.h"
#include <stdlib.h>
#include <stdio.h>

ULONG g_XmoFilterDebugLevel = 2;


TESTDATA g_TestData;

void __cdecl main()
{
    HRESULT hr;

    memset(&g_TestData,0,sizeof(TESTDATA));

    g_TestData.dwFlags = 0;

    DBG_INFO(("**** XmoFilter Test started...Global data at %x\n",&g_TestData));    

    g_TestData.dwFlags = 1;

    BREAK_INTO_DEBUGGER;

    g_TestData.ContextArray[0].dwFlags = g_TestData.dwFlags;

    //Initialize core peripheral port support
    XInitDevices(0,NULL);

    //
    // initialize filter graph
    // define flags here  to determine test configuration
    // The reason we use a context array above is so that you can instantiate multiple
    // xmo filters at the same time and have them run independent of each other.
    // Right now we only use one streaming filter also viewed as chain of streaming objects
    //
    
    hr = Init(0);    
    if (FAILED(hr)) {
        DBG_ERROR(("Init of context failed\n"));
    }

    //
    // instruct routine to poll the filter graph so completed packets can be moved through
    // the streaming chain
    //

    while (TRUE) {

        TransferData(0);

        if (g_TestData.dwFlags == 0) { 
        
            break;
        }
    }    

    Cleanup();
    DBG_INFO(("*** Test stopped, type .reboot now\n"));
    BREAK_INTO_DEBUGGER
   
    Sleep(INFINITE);

}

//
// Init: Creates a streaming graph between 3 XMOs
// A source, a in-between (codec in this case) and a target
// We only support one intermediate xmo, but you could actually
// have multiple ones that manipulate the same stream
// passing packets between them
//

HRESULT
Init(DWORD dwIndex)
{
    PGRAPH_CONTEXT pContext = &g_TestData.ContextArray[dwIndex];
    DWORD dwInsertions;
    DWORD dwDeviceId;
    HRESULT hr = S_OK;
    DWORD count = 0;
    DWORD i;
    DWORD dwMinSize;

    //
    // directory path for audio PCM files
    //

    PWAVEFORMATEX           format = &pContext->wfxAudioFormat;

    PMEDIA_PACKET_CTX       pCtx;
    XMEDIAINFO              xMediaInfo;

    ZeroMemory( &xMediaInfo, sizeof( XMEDIAINFO ) );

    pContext->dwPacketSize = PACKET_SIZE;
    pContext->dwMaxBufferCount = PACKET_CNT;

    //
    // init the audio format..
    //

    format->wFormatTag          = WAVE_FORMAT_PCM;
    format->nSamplesPerSec      = 16000;
    format->nChannels           = 1;
    format->wBitsPerSample      = (WORD) 16;
    format->nBlockAlign         = (16 * format->nChannels / 8);
    format->nAvgBytesPerSec     = format->nSamplesPerSec * format->nBlockAlign;
    format->cbSize              = 0;

    pContext->dwPlayerPort = INVALID_PORT;

    //
    // Wait up to 2 secs to find hawk
    //

    while (count++ < 2) {

        DBG_INFO(("XmoFilter: Sleeping for 1 sec , waiting for hawk\n"));

        Sleep(1000);

        dwInsertions = XGetDevices(XDEVICE_TYPE_VOICE_MICROPHONE);
        if(dwInsertions)
        {
        
            DBG_INFO(("XmoFilter: Insertions 0x%08x\n", dwInsertions));

            for(i=0; i < 32; i++)
            {
                //
                //  Handle Insertions
                //
                if(dwInsertions&(1 << i))
                {
                    DWORD dwPort;
                    if(i < 16)
                    {
                        dwPort = i;
                    } else
                    {
                        dwPort = i-16;
                    }

                    DBG_INFO(("XmoFilter: Hawk in port %d\n", dwPort));
                    pContext->dwPlayerPort = dwPort;
                    count = 0x100;
                    break;

                }
                
                
            } // for loop
        } // if

    } // while

    //
    // instantiate the first XMO in the chain. This will be the hawk USB microphone device
    //

    hr = XVoiceCreateMediaObject(
        XDEVICE_TYPE_VOICE_MICROPHONE,
        pContext->dwPlayerPort,
        PACKET_CNT,
        format,
        &pContext->pSourceXmo);
    
    if (FAILED(hr)) {          
        DBG_ERROR(("XmoFilter: XcreateHawkMediaObject failed with %x",
                  hr));

        return hr;

    }

    //
    // now create a codec or in-between XMO
    //

    hr = XCreateEchoFxMediaObject(
        pContext->dwPacketSize,     // dwDelay (length in bytes of delay line)
        format->wBitsPerSample >> 3, // dwSampleSize (convert to bytes)
        &pContext->pIntermediateXmo
        );

    if (FAILED(hr)) {          
        DBG_ERROR(("XmoFilter: XcreateEchoFxMediaObject failed with %x",
                  hr));

        return hr;

    }

    //
    // now create the target XMO, the end part of the streaming chain
    //

    DSSTREAMDESC dsDesc;

    memset(&dsDesc,0,sizeof(dsDesc));

#ifdef SILVER
    dsDesc.dwSize = sizeof(dsDesc);
#endif // SILVER
    dsDesc.dwFlags = 0;
    dsDesc.dwMaxAttachedPackets = PACKET_CNT;
    dsDesc.lpwfxFormat = format;
#ifdef SILVER
    dwDeviceId = DSDEVID_MCPX;

    hr = DirectSoundCreateStream(
        dwDeviceId,
        &dsDesc,
        (LPDIRECTSOUNDSTREAM *)&pContext->pTargetXmo,
        NULL);
#else // SILVER
	hr = DirectSoundCreateStream( &dsDesc, (LPDIRECTSOUNDSTREAM *)&pContext->pTargetXmo );
#endif // SILVER

    if (FAILED(hr)) {          
        DBG_ERROR(("XmoFilter: DirectSoundCreateXMOStream failed with %x",
                  hr));

        DBG_ERROR(("Init: Could not open a target audio XMO\n"));
        return hr;
    }

    //
    // at this point we should have all the XMOs
    // figure out the minimum packet size we need to pass media buffers
    // in between XMOs. All we do is verifying that our size (PACKET_SIZE)
    // is both larger than the min of all XMOs and its length aligned
    // As a last check we make sure its also larger than the maxLookahead
    //

    ZeroMemory( &xMediaInfo, sizeof( XMEDIAINFO ) );

    if ( pContext->pSourceXmo )
    {
        hr = pContext->pSourceXmo->GetInfo( &xMediaInfo );
        ASSERT( hr == NOERROR );
        pContext->dwMinPacketSize = xMediaInfo.dwOutputSize;

        XMOFILTER_VERIFY_PACKET_SIZE(xMediaInfo,pContext->dwPacketSize);

    }

    if ( pContext->pIntermediateXmo )
    {
        hr = pContext->pIntermediateXmo->GetInfo( &xMediaInfo );
        ASSERT( hr == NOERROR );
        pContext->dwMinPacketSize = max( xMediaInfo.dwInputSize, pContext->dwMinPacketSize );
        XMOFILTER_VERIFY_PACKET_SIZE(xMediaInfo,pContext->dwPacketSize);
    }

    if ( pContext->pTargetXmo )
    {
        hr = pContext->pTargetXmo->GetInfo( &xMediaInfo );
        ASSERT( hr == NOERROR );
        pContext->dwMinPacketSize = max( xMediaInfo.dwOutputSize, pContext->dwMinPacketSize );

        XMOFILTER_VERIFY_PACKET_SIZE(xMediaInfo,pContext->dwPacketSize);
    }

    ASSERT(pContext->pTargetXmo);
    ASSERT(pContext->pSourceXmo);

    InitializeListHead(&pContext->SrcPendingList);
    InitializeListHead(&pContext->DstPendingList);

    //
    // allocate one large buffer for incoming/outgoing data
    // The buffer is going ot be split in small packet segments that
    // will be used to stream data between XMOs
    //

    dwMinSize = MAXBUFSIZE;
    pContext->TransferBuffer = new BYTE[dwMinSize];
    ASSERT(pContext->TransferBuffer);

    memset(pContext->TransferBuffer,0,dwMinSize);

    //
    // allocate a context pool. A context is associated with a packet buffer
    // and used to track the pending packets attached to the source or target XMO
    //

    dwMinSize = (PACKET_CNT*sizeof(MEDIA_PACKET_CTX))+10;
    pContext->PacketContextPool = new BYTE[dwMinSize];
    ASSERT(pContext->PacketContextPool);

    memset(pContext->PacketContextPool,8,dwMinSize);

    //
    // We attach all available packet buffers in the beginning to prime the graph:
    // As each packet is completed by the source XMO, the event is signalled in
    // its packet context and we pass the packet to the next XMO
    //

    for (i=0; i<PACKET_CNT;i++) {

        pCtx = (PMEDIA_PACKET_CTX) (pContext->PacketContextPool+sizeof(MEDIA_PACKET_CTX)*i);

        memset(pCtx,0,sizeof(MEDIA_PACKET_CTX));

        pCtx->dwCompletedSize = pContext->dwPacketSize;
        pCtx->dwStatus = XMEDIAPACKET_STATUS_PENDING;
        pCtx->pBuffer = &pContext->TransferBuffer[i*PACKET_SIZE];

        InsertContext(pContext,
                      pCtx,
                      pContext->pSourceXmo,
                      TRUE);

    }

    return S_OK;

}

VOID
Cleanup()
{
    PGRAPH_CONTEXT pContext;
    ULONG i=0;
    DBG_INFO(("Test cleanup. Deleting media objects.\n"));

    for (i=0;i<2;i++) {
        pContext = &g_TestData.ContextArray[i];

        //
        // delete any xmos...
        //
    
        if (pContext->pSourceXmo) {
            pContext->pSourceXmo->Release();
        }
    
        if (pContext->pIntermediateXmo) {
            pContext->pIntermediateXmo->Release();
        }
    
        if (pContext->pTargetXmo) {
            pContext->pTargetXmo->Release();
        }


    }

}



HRESULT
TransferData(
    DWORD dwIndex
    )
{
    PGRAPH_CONTEXT pContext = &g_TestData.ContextArray[dwIndex];
    HRESULT hr=S_OK;
    DWORD index = 0;
    XMEDIAPACKET xmb;
    DWORD dwSize;    
    PMEDIA_PACKET_CTX pSrcCtx, pDstCtx;
    HANDLE hArray[2];

    //
    // setup media buffer
    //

    memset(&xmb,0,sizeof(xmb));

    dwSize = 0;
    pSrcCtx = NULL;
    pDstCtx = NULL;

    //
    // Wait for the oldest buffers submitted to the source and target xmo
    // oldest buffers are at the head, since we insert at the tail, remove from head
    //

    if (!IsListEmpty(&pContext->SrcPendingList)) {

        pSrcCtx = (PMEDIA_PACKET_CTX) pContext->SrcPendingList.Flink; // head

    }

    if (!IsListEmpty(&pContext->DstPendingList)) {

        pDstCtx = (PMEDIA_PACKET_CTX) pContext->DstPendingList.Flink; //head

    }        

    if (pSrcCtx && 
        (pSrcCtx->dwStatus != XMEDIAPACKET_STATUS_PENDING)) {

        //
        // a src buffer completed.
        // Remove this context from the Src pending list.
        // Send it to a codec, if present, then send it to the Target xmo
        //

        RemoveEntryList(&pSrcCtx->ListEntry);

        //
        // Before we pass it on, set the context packet to amount of data we received
        //

        xmb.pvBuffer = pSrcCtx->pBuffer;            
        xmb.dwMaxSize = pSrcCtx->dwCompletedSize;        
        xmb.hCompletionEvent = NULL;

        DBG_SPAM(("Context %x received %x bytes, xmb %x\n",pSrcCtx, xmb.dwMaxSize, &xmb));
        ASSERT(xmb.dwMaxSize);

        pSrcCtx->dwCompletedSize = 0;
        pSrcCtx->dwStatus = XMEDIAPACKET_STATUS_PENDING;

        if (pContext->pIntermediateXmo) {

            //
            // NOTE: its ok to pass a pointer to a stack variable since
            // middle XMO is sync!!! Never do this with async XMOs!!!
            // Note that we pass the same media packet description twice
            // THis is  because the particular intermediate effect XMO is in place
            // and it operates on a single buffer
            //

            dwSize = xmb.dwMaxSize;
            xmb.pdwCompletedSize = &dwSize;

            hr = pContext->pIntermediateXmo->Process(
                &xmb,
                &xmb);

            ASSERT(SUCCEEDED(hr));
            ASSERT(dwSize <=PACKET_SIZE);

            pSrcCtx->dwCompletedSize = dwSize;

        } else { // if intermediate xmo

            pSrcCtx->dwCompletedSize = pContext->dwPacketSize;

        }

        //
        // pass media buffer to the target xmo.
        //

        InsertContext(pContext,
                      pSrcCtx,
                      pContext->pTargetXmo,
                      FALSE);

    }

    //
    // check if destination buffer xmo was also done..
    //

    if (pDstCtx && 
        (pDstCtx->dwStatus != XMEDIAPACKET_STATUS_PENDING)) {        

        DBG_SPAM(("Target XMO completed context %x\n",pDstCtx));

        RemoveEntryList(&pDstCtx->ListEntry);

        ASSERT(pDstCtx->dwCompletedSize != 0);

        //
        // the target XMO is done with a buffer.
        //

        pDstCtx->dwCompletedSize = PACKET_SIZE;
        InsertContext(pContext,
                      pDstCtx,
                      pContext->pSourceXmo,
                      TRUE);


    } 

    return hr;

}

//
// helper function
// InsertContext prepares a packet context and attaches it to the source
// or target XMO
//

VOID
InsertContext(
    PGRAPH_CONTEXT pContext,
    PMEDIA_PACKET_CTX pCtx,
    XMediaObject *pXmo,
    BOOL fSource
    )
{
    XMEDIAPACKET xmb;
    HRESULT hr;

    memset(&xmb,0,sizeof(xmb));            

    xmb.pvBuffer = pCtx->pBuffer;            
    xmb.dwMaxSize = pCtx->dwCompletedSize;

    pCtx->dwCompletedSize = 0;
    pCtx->dwStatus = XMEDIAPACKET_STATUS_PENDING;

    xmb.hCompletionEvent = NULL;
    xmb.pdwCompletedSize = &pCtx->dwCompletedSize;
    xmb.pdwStatus = &pCtx->dwStatus;

    if (fSource) {

        //
        // attach to the source XMO (first xmo in the chain)
        //

        hr = pXmo->Process(
            NULL,
            &xmb);

        InsertTailList(&pContext->SrcPendingList, &pCtx->ListEntry);


    } else {

        //
        // attach to the target XMO (first xmo in the chain)
        //

        hr = pXmo->Process(
            &xmb,
            NULL);

        InsertTailList(&pContext->DstPendingList, &pCtx->ListEntry);

    }

    if (FAILED(hr)) {

        DBG_ERROR(("InsertContext: Xmo %x, Process failed with hr = %x\n",hr));
        BREAK_INTO_DEBUGGER;
    }

}


