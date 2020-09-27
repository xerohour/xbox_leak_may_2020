
/*==========================================================================
 *
 *  Copyright (C) 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/1/2000   georgioc created
 *  2/25/2001   georgioc added p2p voice xmos
 *
 ***************************************************************************/

#include "netstream.h"

#include <stdlib.h>
#include <stdio.h>

ULONG g_NetStrmDebugLevel = 2;

VOID
InsertContext(
    PGRAPH_CONTEXT pContext,
    PMEDIA_BUFFER_CTX pCtx,
    XMediaObject *pXmo,
    BOOL fSource
    );


NETSTREAM_GLOBAL_DATA g_Data;

VOID __cdecl main()
{
    WSADATA wsadata;
    INT err;

    memset(&g_Data,0,sizeof(NETSTREAM_GLOBAL_DATA));

    g_Data.dwFlags = 0;

    DBG_INFO(("**** NetStream Test started...Global data at %x\n",&g_Data));    
    DBG_INFO(("**** To receive net data ed %x %x\n",
             &g_Data,     
             TESTF_NET_RECV));

    DBG_INFO(("**** To send net data ed %x %x\n",
             &g_Data,     
             TESTF_NET_SEND));

    g_Data.dwFlags = TESTF_NET_SEND | TESTF_USE_CODEC;

    BREAK_INTO_DEBUGGER;

    //
    //  Initialize core peripheral port support
    //

    XInitDevices(0,NULL);
    
    //
    // load network stack
    //

    DBG_INFO(("NetStream: Loading XBox network stack...\n"));
    err = XnetInitialize(NULL, TRUE);
    if (err != NO_ERROR) {
        DBG_ERROR(("NetStream: XnetInitialize failed with %x",
                  err));
        return;
    }

    err = WSAStartup(WINSOCK_VERSION, &wsadata);
    if (err != NO_ERROR) {
        DBG_ERROR(("NetStream: WSAStartup failed with %x",
                  err));
        return;
    }

    g_Data.ContextArray[NET_RECV_CONTEXT].dwFlags = g_Data.dwFlags;
    g_Data.ContextArray[NET_SEND_CONTEXT].dwFlags = g_Data.dwFlags;

    //
    // initialize full duplex filter graph and net stack.
    // define flags here  to determine test configuration
    // first initialize the local capture graph, then the target graph
    //
    
    if (g_Data.dwFlags & TESTF_NET_RECV) {

        err = Init(NET_RECV_CONTEXT);    
        if (FAILED(err)) {
            DBG_ERROR(("Init of net receive context failed, resorting to half duplex\n"));
            g_Data.dwFlags &= ~TESTF_NET_RECV;
        }
    
    }

    if (g_Data.dwFlags & TESTF_NET_SEND) {

        err = Init(NET_SEND_CONTEXT);    
        if (FAILED(err)) {
            DBG_ERROR(("Init of net send context failed, resorting to half duplex\n"));
            g_Data.dwFlags &= ~TESTF_NET_SEND;
        }

    }

    //
    // instruct routine to poll each filter graph (0 for timeout means dont block)
    //

    while (TRUE) {

        if (g_Data.dwFlags & TESTF_NET_RECV) {
            TransferData(NET_RECV_CONTEXT, 0);
        } 
        
        if (g_Data.dwFlags & TESTF_NET_SEND) {
            TransferData(NET_SEND_CONTEXT, 0);
        }        

        if (g_Data.dwFlags == 0) { 
        
            break;
        }
    }    

    Cleanup();
    DBG_INFO(("*** Test stopped, type .reboot now\n"));
    BREAK_INTO_DEBUGGER
   
    Sleep(INFINITE);

    return;

}

//
// Init: Creates a streaming graph between 3 XMOs
// A source, a in-between (codec in this case) and a target
// Depending on whcih net mode we are in, the source and target change
// In Client Mode:
//      Source must be the NetXmo in client mode
//      IntermediaXmo can be NULL
//      Target XMO must be an Audio device (hawk or mcpx)
// In Server Mode:
//      SourceXmo can be NULL
//      pWaveFile can be NULL if sourceXmo != NULL and vice versa
//      pIntermediateXmo can be NULL
//      pTargetXmo must be the NetXmo initialized in server mode
// We only support one target and one source, but you could actually
// have multiple sources and targets that manipulate the same stream
// all you have to do is call multiple XMos, one after the other
// using the same XMediaBuffer (in sync mode) or multiple copies (async)
// In case there is no hawk microphone device, we will read data from a file
// and stream that over the net.
// If there is no hawk output device and we are in client mode, we will output 
// to the MCPX
//

HRESULT
Init(DWORD dwIndex)
{
    PGRAPH_CONTEXT pContext = &g_Data.ContextArray[dwIndex];
    DWORD dwInsertions;
    PCHAR pszAddress;
    CHAR localAddressBuffer[256];
    struct in_addr localAddress;
    DWORD dwMinSize, dwDeviceId;
    DWORD err = S_OK;
    DWORD count = 0;
    DWORD i;

    PWAVEFORMATEX           format = &pContext->wfxAudioFormat;

    PMEDIA_BUFFER_CTX       pCtx;
    XMEDIAINFO              xMediaInfo;

    union {

        WAVEFORMATEX wfx;
#ifdef SILVER
        IMAADPCMWAVEFORMAT wfxAdpcm;
#else // SILVER
		XBOXADPCMWAVEFORMAT wfxAdpcm;
#endif // SILVER
        VOICECODECWAVEFORMAT wfxVox;
    };

#ifdef SILVER
    ZeroMemory( &wfxAdpcm, sizeof( IMAADPCMWAVEFORMAT ) );
#else // SILVER
	ZeroMemory( &wfxAdpcm, sizeof( XBOXADPCMWAVEFORMAT ) );
#endif // SILVER

    ZeroMemory( &xMediaInfo, sizeof( XMEDIAINFO ) );

    pContext->dwPacketSize = PACKET_SIZE;
    pContext->dwMaxBufferCount = PACKET_CNT;

    //
    // init the audio format..
    //

    format->wFormatTag          = WAVE_FORMAT_PCM;
    format->nSamplesPerSec      = 8000;
    format->nChannels           = 1;
    format->wBitsPerSample      = (WORD) 16;
    format->nBlockAlign         = (16 * format->nChannels / 8);
    format->nAvgBytesPerSec     = format->nSamplesPerSec * format->nBlockAlign;
    format->cbSize              = 0;

    //
    // figure out which machine we are running on...
    //

    do {

        XnetGetIpAddress(&localAddress);
        pszAddress = inet_ntoa(localAddress);
        memcpy(localAddressBuffer,pszAddress,sizeof(localAddressBuffer));

        i = RtlCompareMemory(localAddressBuffer,LOOPADDRESS,sizeof(LOOPADDRESS));
        if (i!=sizeof(LOOPADDRESS)) {
            DBG_ERROR(("**** NetStream: Local address is %s ****\n", localAddressBuffer));
            break;
        }

    } while (TRUE);

    pContext->dwPlayerPort = INVALID_PORT;

    //
    // Wait up to 2 secs to find hawk
    //

    while (count++ < 2) {

        DBG_INFO(("Netstream: Sleeping for 1 sec , waiting for hawk\n"));

        dwInsertions = XGetDevices(XDEVICE_TYPE_VOICE_MICROPHONE);
        if(dwInsertions)
        {
        
            DBG_INFO(("Netstream: Insertions 0x%08x\n", dwInsertions));

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

                    DBG_INFO(("Netstream: Hawk in port %d\n", dwPort));
                    pContext->dwPlayerPort = dwPort;
                    count = 0x100;
                    break;

                }
                
                
            } // for loop
        } // if

        Sleep(1000);

    } // while

    dwMinSize = RtlCompareMemory(inet_ntoa(localAddress),TESTADDR_RIGHT,sizeof(TESTADDR_RIGHT));

    //
    // initialize the source XMO
    //  

    if (dwIndex == NET_RECV_CONTEXT) {
        
        //
        // Instantiate Net xmo
        // Figure which machine we a running so we connect to the OTHER machine
        // Pass the max packet size, not the real packet size after encoding
        // in case we want to send raw data
        //
                
        err = XnetCreateMediaObject(
            &pContext->pSourceXmo,
            INVALID_SOCKET,
            (dwMinSize == sizeof(TESTADDR_RIGHT) ? TESTADDR_LEFT : TESTADDR_RIGHT),
            TESTPORT,
            PACKET_SIZE,
            0);

        
        if (FAILED(err)) {          
            DBG_ERROR(("NetStream: XcreateNetMediaObject failed with %x",
                      err));

            goto exitError;         
        }
        
    }
    
    if (dwIndex == NET_SEND_CONTEXT) {
                        
        err = XVoiceCreateMediaObject(
            XDEVICE_TYPE_VOICE_MICROPHONE,
            pContext->dwPlayerPort,
            PACKET_CNT,
            format,
            &pContext->pSourceXmo);
        
        if (FAILED(err)) {          
            DBG_ERROR(("NetStream: XcreateHawkMediaObject failed with %x",
                      err));

            err = E_FAIL;
            goto exitError;

        }
        
    }

    //
    // now create a codec or in-between XMO
    //

    if (pContext->dwFlags & TESTF_USE_CODEC) {

        DWORD dwFlags;

        //
        // if we are testing a net client, it means we
        // are receiving compressed data from the server
        // If we are in erver mode, we are sending data so we need
        // to encode it first (thus create an encoder)
        // assumption is that the other side encoded with the same codec it was
        // specified for this side..
        //

        dwFlags = ( dwIndex == NET_SEND_CONTEXT ) ? 1 : 0;     

        if ( dwFlags )
        {
            DBG_INFO(("creating voice encoder\n"));

            err = XVoiceEncoderCreateMediaObject(
                FALSE, // manual mode
                WAVE_FORMAT_VOXWARE_SC06,
                format,
                20, // voice activation threshold
                &pContext->pIntermediateXmo
                );

        }
        else
        {

            DBG_INFO(("creating voice decoder\n"));

            err = XVoiceDecoderCreateMediaObject(
                0, // zero latency
                WAVE_FORMAT_VOXWARE_SC06,
                format,
                &pContext->pIntermediateXmo
                );

        }

        if (FAILED(err)) {          

            DBG_ERROR(("NetStream: voice codec failed with %x",
                      err));
            goto exitError;         
        }

        //
        // if the codec XMO is in place dont pass a DST temp buffer...
        //

        pContext->pIntermediateXmo->GetInfo( &xMediaInfo );

        if (xMediaInfo.dwFlags & XMO_STREAMF_IN_PLACE) {

            DBG_INFO(("TransferData: Inbetween xmo %x is in place\n",
                     pContext->pIntermediateXmo));

        } else {
                                               
            memset(&pContext->DstMediaBuffer,
                   0,
                   sizeof(XMEDIAPACKET));

            pContext->DstMediaBuffer.dwMaxSize = PACKET_SIZE;
            pContext->DstMediaBuffer.pvBuffer = new BYTE[PACKET_SIZE];

            ASSERT(pContext->DstMediaBuffer.pvBuffer);

            DBG_INFO(("TransferData: Inbetween xmo %x is not in place, using temp buffer\n",
                     pContext->pIntermediateXmo));

        }

    }

    //
    // init the target XMOs
    //

    if (dwIndex == NET_SEND_CONTEXT) {
        
        //
        // the recv context already has a net XMO instantiated, use it
        // the net xmo can be used for both send and receive at the same time
        //

        pContext->pTargetXmo = g_Data.ContextArray[NET_RECV_CONTEXT].pSourceXmo;
        if (!pContext->pTargetXmo) {

            //
            // half duplex mode
            //

            err = XnetCreateMediaObject(
                &pContext->pTargetXmo,
                INVALID_SOCKET,
                (dwMinSize == sizeof(TESTADDR_RIGHT) ? TESTADDR_LEFT : TESTADDR_RIGHT),
                TESTPORT,
                PACKET_SIZE,
                0);
    
            
            if (FAILED(err)) {          
                DBG_ERROR(("NetStream: XnetCreateMediaobject failed with %x",
                          err));
                goto exitError;         
            }


        }
        
    } else {
#ifdef SILVER
        dwDeviceId = DSDEVID_VOICE(pContext->dwPlayerPort);
#else // SILVER
		dwDeviceId = 0;
#endif // SILVER

        //
        // net recv, we are receiving data and sending to an
        // audio device. default is mcpx
        //

        if (pContext->dwPlayerPort != INVALID_PORT) {
            
            //
            // for now just open hawk direct..
            //

            err = XVoiceCreateMediaObject(
                XDEVICE_TYPE_VOICE_HEADPHONE,
                pContext->dwPlayerPort,
                PACKET_CNT,
                format,
                &pContext->pTargetXmo);
            
            if (err) {
                DBG_ERROR(("NetStream: XVoiceCreateMediaobject failed with %x\n",
                          err));
            }
            

        } else {

            //
            // default to MCPX
            //

            err = E_FAIL;

        }

        if (FAILED(err)) {          
            
            //
            // if we tried to open the hawk output and it failed
            // try the mcpx
            //

#ifdef SILVER
            if (dwDeviceId != DSDEVID_MCPX) {
#else // SILVER
			if ( dwDeviceId != 0 ) {
#endif // SILVER

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
#else // SILVER
				dwDeviceId = 0;
#endif // SILVER

#ifdef SILVER
                err = DirectSoundCreateStream(
                    dwDeviceId,
                    &dsDesc,
                    (LPDIRECTSOUNDSTREAM *)&pContext->pTargetXmo,
                    NULL);
#else // SILVER
				err = DirectSoundCreateStream( &dsDesc, (LPDIRECTSOUNDSTREAM *)&pContext->pTargetXmo );
#endif // SILVER

                if (FAILED(err)) {          
                    DBG_ERROR(("NetStream: DirectSoundCreateXMOStream failed with %x",
                              err));

                    DBG_ERROR(("Init: Could not open a target audio XMO\n"));
                    goto exitError;
                }

            } else {

                DBG_WARN(("Init: Could not open a target audio XMO\n"));
                pContext->pTargetXmo = NULL;

            }

        }

    }

#if DBG

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
        err = pContext->pSourceXmo->GetInfo( &xMediaInfo );
        ASSERT( err == NOERROR );
        pContext->dwMinPacketSize = xMediaInfo.dwOutputSize;

        NETSTRM_VERIFY_PACKET_SIZE(xMediaInfo,pContext->dwPacketSize);

    }

    if ( pContext->pIntermediateXmo )
    {
        err = pContext->pIntermediateXmo->GetInfo( &xMediaInfo );
        ASSERT( err == NOERROR );
        pContext->dwMinPacketSize = max( xMediaInfo.dwInputSize, pContext->dwMinPacketSize );

        //NETSTRM_VERIFY_PACKET_SIZE(xMediaInfo,pContext->dwPacketSize);
    }

    if ( pContext->pTargetXmo )
    {
        err = pContext->pTargetXmo->GetInfo( &xMediaInfo );
        ASSERT( err == NOERROR );
        pContext->dwMinPacketSize = max( xMediaInfo.dwOutputSize, pContext->dwMinPacketSize );

        NETSTRM_VERIFY_PACKET_SIZE(xMediaInfo,pContext->dwPacketSize);
    }

#endif

    pContext->dwPacketSize = PACKET_SIZE;

    ASSERT(pContext->pTargetXmo);
    ASSERT(pContext->pSourceXmo);

    InitializeListHead(&pContext->SrcPendingList);
    InitializeListHead(&pContext->DstPendingList);

    //
    // allocate one large buffer for incoming/outgoing data
    //

    dwMinSize = MAXBUFSIZE;
    pContext->TransferBuffer = new BYTE[dwMinSize];
    ASSERT(pContext->TransferBuffer);

    memset(pContext->TransferBuffer,0,dwMinSize);

    //
    // allocate a context pool
    //

    dwMinSize = (PACKET_CNT*sizeof(MEDIA_BUFFER_CTX))+10;
    pContext->PacketContextPool = new BYTE[dwMinSize];
    ASSERT(pContext->PacketContextPool);

    memset(pContext->PacketContextPool,8,dwMinSize);

    //
    // we attach all available buffers in the beginning to prime the graph:
    // as each buffer is completed by the source, the event is signalled and
    // we pass the media buffer to the next XMO
    //

    for (i=0; i<PACKET_CNT;i++) {

        pCtx = (PMEDIA_BUFFER_CTX) (pContext->PacketContextPool+sizeof(MEDIA_BUFFER_CTX)*i);

        memset(pCtx,0,sizeof(MEDIA_BUFFER_CTX));

        pCtx->dwCompletedSize = PACKET_SIZE;
        pCtx->dwStatus = XMEDIAPACKET_STATUS_PENDING;
        pCtx->hCompletionEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
        pCtx->pBuffer = &pContext->TransferBuffer[i*PACKET_SIZE];

        ASSERT(pCtx->hCompletionEvent);

        InsertContext(pContext,
                      pCtx,
                      pContext->pSourceXmo,
                      TRUE);

    }

    return S_OK;

exitError:

    return err;

}

VOID
Cleanup()
{
    PGRAPH_CONTEXT pContext;
    ULONG i=0;
    DBG_INFO(("Test cleanup. Deleting media objects.\n"));

    for (i=0;i<2;i++) {
        pContext = &g_Data.ContextArray[i];

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


    DBG_ERROR(("Unloading XBox network stack...\n"));
    WSACleanup();

    XnetCleanup();

}



HRESULT
TransferData(
    DWORD dwIndex,
    DWORD dwTimeout
    )
{
    PGRAPH_CONTEXT pContext = &g_Data.ContextArray[dwIndex];
    HRESULT hr=S_OK;
    DWORD index = 0;
    XMEDIAPACKET xmb;
    DWORD dwSize;    
    PMEDIA_BUFFER_CTX pSrcCtx, pDstCtx;
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

        pSrcCtx = (PMEDIA_BUFFER_CTX) pContext->SrcPendingList.Flink; // head
        hArray[0] = pSrcCtx->hCompletionEvent;
        dwSize = 1;

    }

    if (!IsListEmpty(&pContext->DstPendingList)) {

        pDstCtx = (PMEDIA_BUFFER_CTX) pContext->DstPendingList.Flink; //head
        hArray[dwSize] = pDstCtx->hCompletionEvent;
        dwSize++;

    }        

    if (dwSize) {

        index = WaitForMultipleObjects(dwSize,
                                       hArray,
                                       FALSE,
                                       dwTimeout);

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
        // Before we pass it on, set xmb to amount of data we received
        //

        xmb.pvBuffer = pSrcCtx->pBuffer;            
        xmb.dwMaxSize = pSrcCtx->dwCompletedSize;        
        xmb.hCompletionEvent = pSrcCtx->hCompletionEvent;

        DBG_SPAM(("Context %x received %x bytes, xmb %x\n",pSrcCtx, xmb.dwMaxSize, &xmb));
        ASSERT(xmb.dwMaxSize);

        pSrcCtx->dwCompletedSize = 0;
        pSrcCtx->dwStatus = XMEDIAPACKET_STATUS_PENDING;

        if (pContext->pIntermediateXmo) {

            if (pContext->DstMediaBuffer.pvBuffer) {

                //
                // for debug reasons, find out output byte count
                // NOTE: its ok to pass a pointer to a stack variable since
                // middle XMO is sync!!! Never do this with async XMOs!!!
                //

                pContext->DstMediaBuffer.pdwCompletedSize = &dwSize;
                dwSize = 0;

                hr = pContext->pIntermediateXmo->Process(
                    &xmb,
                    &pContext->DstMediaBuffer);


                ASSERT(dwSize <=PACKET_SIZE);

                memcpy(xmb.pvBuffer,
                       pContext->DstMediaBuffer.pvBuffer,
                       dwSize);

                DBG_SPAM(("Conversion produced %x bytes\n",dwSize));

            } else {

                //
                // middle xmo is IN_PLACE
                //

                hr = pContext->pIntermediateXmo->Process(
                    NULL,
                    &xmb);

            }

            pSrcCtx->dwCompletedSize = dwSize;
            ASSERT(!NETSTRM_FAILED(hr));

        } else { // if intermediate xmo

            pSrcCtx->dwCompletedSize = PACKET_SIZE;

        }

        //
        // if the voice decoder returned 0 bytes it means
        // that the received packet was silence..
        // We have two choices:
        // 1) pass to the target xmo a packet initialized with silence(using some
        // single preallocated silence buffer)
        // 2) dont pass anything to the target xmo and just requeue the completed context
        // to the source list.
        // If we choose 1), we are keeping the average stream throughput fairly constant
        // If we choose 2) we use less cpu time since no need for the target to queue up
        // the packet, notify us when its complete etc.
        // Here I chose 2) because its simple. You might want to use 1) to keep your filter
        // graph more deterministic and also allow for some jitter-smoothing
        //

        if (dwSize == 0) {

            pSrcCtx->dwCompletedSize = PACKET_SIZE;

            //
            // re-attach packet to source
            //

            InsertContext(pContext,
                          pSrcCtx,
                          pContext->pSourceXmo,
                          TRUE);


        } else {

            //
            // pass media buffer to the target xmo.
            //

            InsertContext(pContext,
                          pSrcCtx,
                          pContext->pTargetXmo,
                          FALSE);

        }

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
// helper functions

VOID
InsertContext(
    PGRAPH_CONTEXT pContext,
    PMEDIA_BUFFER_CTX pCtx,
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

    xmb.hCompletionEvent = pCtx->hCompletionEvent;
    xmb.pdwCompletedSize = &pCtx->dwCompletedSize;
    xmb.pdwStatus = &pCtx->dwStatus;

    if (fSource) {

        hr = pXmo->Process(
            NULL,
            &xmb);

        InsertTailList(&pContext->SrcPendingList, &pCtx->ListEntry);

    } else {

        hr = pXmo->Process(
            &xmb,
            NULL);

        InsertTailList(&pContext->DstPendingList, &pCtx->ListEntry);

    }

    if (NETSTRM_FAILED(hr)) {

        DBG_ERROR(("InsertContext: Xmo %x, Process failed with hr = %x\n",hr));

    }

}


