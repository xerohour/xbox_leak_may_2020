//-----------------------------------------------------------------------------
// File: Voice.cpp
//
// Hist: 08.08.01 - New for Aug M1 release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Voice.h"

//////////////////////////////////////////////////////////////////////////////
//
// Implementation of CCommunicator - abstract base class 
//
//////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Name: CCommunicator
// Desc: Constructor
//-----------------------------------------------------------------------------
CCommunicator::CCommunicator(
            WORD    wHeadsetPort,
            HANDLE  hEvtMicInput,
            WORD    wCodecTag,
            WORD    wVoiceSamplingRate,
            WORD    wCodecSamplingRate,
            WORD    wVoiceBufferTime,
            WORD    wVoiceBufferCount )
{
#ifdef _WRITE_LOG
    m_hFileRawPCMData   = NULL;
    m_hFileCompressed   = NULL;
    m_hFileDecompressed = NULL; 
#endif

    m_wHeadsetPort                  = wHeadsetPort;
    m_pMicrophone                   = NULL;
    m_pHeadphone                    = NULL;
    m_bCreateMicrophoneEvent        = FALSE;
    m_pEncoder                      = NULL;
    m_hHeadphoneWorkEvent           = INVALID_HANDLE_VALUE;
    m_pBeginMicrophoneQueuedBuffers = NULL;
    m_pBeginHeadphoneQueuedBuffers  = NULL;
    m_hMicrophoneWorkEvent          = hEvtMicInput;
    m_wVoiceSamplingRate            = wVoiceSamplingRate;
    m_wCodecSamplingRate            = wCodecSamplingRate;

    assert( (WAVE_FORMAT_VOXWARE_SC06 == wCodecTag) || (WAVE_FORMAT_VOXWARE_SC03 == wCodecTag));
    m_wCodecTag = wCodecTag;

    m_wVoiceBufferTime      = wVoiceBufferTime;
    m_wVoiceBufferSize      = wVoiceBufferTime * (((WORD)( wVoiceSamplingRate / 1000.0 )) * 2 );
    m_wVoiceBuffersCount    = wVoiceBufferCount;
    m_wCodecBufferSize      = 0;

    m_CodecMediaBuffer.pvBuffer = NULL;
}




//-----------------------------------------------------------------------------
// Name: ~CCommunicator
// Desc: Destructor
//-----------------------------------------------------------------------------
CCommunicator::~CCommunicator()
{
    CCommunicator::Cleanup();
}




//-----------------------------------------------------------------------------
// Name: Cleanup
// Desc: Called at destruction time or when the communicator is removed
//-----------------------------------------------------------------------------
void CCommunicator::Cleanup()
{
    if(m_pMicrophone)
    {
        m_pMicrophone->Release();
        m_pMicrophone = NULL;
    }    

    if(m_pHeadphone)
    {
        m_pHeadphone->Release();
        m_pHeadphone = NULL;
    }

    if(m_pEncoder)
    {
        m_pEncoder->Release();
        m_pEncoder = NULL;
    }

    if ((m_bCreateMicrophoneEvent) && (INVALID_HANDLE_VALUE != m_hMicrophoneWorkEvent)) 
    {
        CloseHandle(m_hMicrophoneWorkEvent);
        m_hMicrophoneWorkEvent = INVALID_HANDLE_VALUE;
        m_bCreateMicrophoneEvent = FALSE;
    }

    if (INVALID_HANDLE_VALUE != m_hHeadphoneWorkEvent) 
    {
        CloseHandle(m_hHeadphoneWorkEvent);
        m_hHeadphoneWorkEvent = INVALID_HANDLE_VALUE;
    }

    if( NULL != m_pBeginMicrophoneQueuedBuffers )
    {
        DeleteQueue(m_pBeginMicrophoneQueuedBuffers);
        m_pBeginMicrophoneQueuedBuffers = NULL;

    }

    if( NULL != m_pBeginHeadphoneQueuedBuffers )
    {
        DeleteQueue(m_pBeginHeadphoneQueuedBuffers);
        m_pBeginHeadphoneQueuedBuffers = NULL;
    }

    if( NULL != m_CodecMediaBuffer.pvBuffer )
    {
        delete [] m_CodecMediaBuffer.pvBuffer;
        m_CodecMediaBuffer.pvBuffer = NULL;
    }

#ifdef _WRITE_LOG
    CloseFile(&m_hFileRawPCMData);
    CloseFile(&m_hFileCompressed);
    CloseFile(&m_hFileDecompressed);
#endif
}




//-----------------------------------------------------------------------------
// Name: CreateQueue
// Desc: Create a queue of MEDIA_PACKET_LIST of a given size and initialize it
//-----------------------------------------------------------------------------
void CCommunicator::CreateQueue(MEDIA_PACKET_LIST **pQueue , WORD wBufferSize, WORD wCount , DWORD dwStatus)
{
    MEDIA_PACKET_LIST *pTmp = NULL;
    MEDIA_PACKET_LIST *pHead = NULL;
    
    // Create linked-list
    for( WORD i = 0 ; i <  wCount ; i++ )
    {
        if( pTmp )
        {
            // Tacking on to end...
            pTmp->pNext = new MEDIA_PACKET_LIST(wBufferSize , dwStatus);
            pTmp = pTmp->pNext;
        }
        else
        {
            // First node
            pTmp = new MEDIA_PACKET_LIST(wBufferSize , dwStatus);
            pHead = pTmp;
        }

        // Check for memory allocation failures
        if( NULL == pTmp )
        {
            break;
        }
        if( NULL == (pTmp->MediaPacket).pvBuffer )
        {
            delete pTmp;
            pTmp = NULL;
            break;
        }
    }

    if(NULL == pTmp)
    {
        // Something went wrong, release allocated memory
        pTmp = pHead;

        while(NULL != pTmp)
        {
            pHead = pTmp->pNext;
            delete pTmp;
            pTmp = pHead;
        }
        *pQueue = NULL;
    }
    else
    {
        // Close the loop
        pTmp->pNext = pHead;
        *pQueue = pHead;
    }
}




//-----------------------------------------------------------------------------
// Name: DeleteQueue
// Desc: Delete a queue of MEDIA_PACKET_LIST 
//-----------------------------------------------------------------------------
void CCommunicator::DeleteQueue(MEDIA_PACKET_LIST *pQueue)
{
    assert(pQueue->pNext != pQueue);

    MEDIA_PACKET_LIST *pTmpBuff = pQueue;

    // Break the loop
    pQueue = pQueue->pNext;
    pTmpBuff->pNext = NULL;

    while(NULL != pQueue)
    {
        pTmpBuff = pQueue->pNext;
        delete pQueue;
        pQueue = pTmpBuff;
    }

}




//-----------------------------------------------------------------------------
// Name: Inserted
// Desc: Called when a communicator is newly detected in this slot, does all initializations
//-----------------------------------------------------------------------------
HRESULT CCommunicator::Inserted()
{
    HRESULT         hr;
    WAVEFORMATEX    wfxFormat;
    
    OUTPUT_DEBUG_STRING( "CCommunicator::Inserted() - Xbox Communicator Inserted\n" );

    // Initialize wave format
    wfxFormat.wFormatTag = WAVE_FORMAT_PCM;
    wfxFormat.nChannels = 1;
    wfxFormat.nSamplesPerSec = m_wVoiceSamplingRate;
    wfxFormat.nAvgBytesPerSec = 2 * m_wVoiceSamplingRate;
    wfxFormat.nBlockAlign = 2;
    wfxFormat.wBitsPerSample = 16;
    wfxFormat.cbSize = 0;

    //  Create a microphone and a headphone
    hr = XVoiceCreateMediaObject( XDEVICE_TYPE_VOICE_MICROPHONE, 
                                  m_wHeadsetPort, 
                                  m_wVoiceBuffersCount,
                                  &wfxFormat, 
                                  &m_pMicrophone );
    if( FAILED( hr ) )
    {
        OUTPUT_DEBUG_STRING( "CCommunicator::Inserted() - Microphone creation failed\n" );
        return hr;
    }

    hr = XVoiceCreateMediaObject( XDEVICE_TYPE_VOICE_HEADPHONE, 
                                  m_wHeadsetPort, 
                                  m_wVoiceBuffersCount,
                                  &wfxFormat, 
                                  &m_pHeadphone );

    if( FAILED( hr ) )
    {
        OUTPUT_DEBUG_STRING( "CCommunicator::Inserted() - Headphone creation failed\n" );
        return hr;
    }

    hr = XVoiceEncoderCreateMediaObject( FALSE,         // manual mode
                                         m_wCodecTag,
                                         &wfxFormat,
                                         20,            // voice activation threshold (0 - 255)
                                         &m_pEncoder );

    if( FAILED( hr ) )
    {
        OUTPUT_DEBUG_STRING( "CCommunicator::Inserted() - Voice encoder creation failed\n" );
        return hr;
    }

    // Set the codec buffer size taking into account the codec
    XVoiceGetCodecBufferSize( m_pEncoder, m_wVoiceBufferSize , &m_wCodecBufferSize);

    // Events for microphone and headphone working...
    if(INVALID_HANDLE_VALUE == m_hMicrophoneWorkEvent)
    {
        m_hMicrophoneWorkEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        m_bCreateMicrophoneEvent = TRUE;
    }
    m_hHeadphoneWorkEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    

    // Create the buffers for microphone
    CreateQueue( &m_pBeginMicrophoneQueuedBuffers, 
                 m_wVoiceBufferSize, 
                 m_wVoiceBuffersCount, 
                 (DWORD)XMEDIAPACKET_STATUS_PENDING );
    if( NULL ==  m_pBeginMicrophoneQueuedBuffers)
    {
        OUTPUT_DEBUG_STRING( "CCommunicator::Inserted() - Not enough memory for microphone buffers!\n" );
        return E_OUTOFMEMORY;
    }

    // Create the buffers for headphone
    CreateQueue( &m_pBeginHeadphoneQueuedBuffers, 
                 m_wVoiceBufferSize, 
                 m_wVoiceBuffersCount, 
                 (DWORD)XMEDIAPACKET_STATUS_SUCCESS );
    if( NULL ==  m_pBeginHeadphoneQueuedBuffers)
    {
        OUTPUT_DEBUG_STRING( "CCommunicator::Inserted() - Not enough memory for headphone buffers!\n" );
        return E_OUTOFMEMORY;
    }

    // Create the buffer used by the codec for compression
    memset(&m_CodecMediaBuffer, 0, sizeof(XMEDIAPACKET));
    m_CodecMediaBuffer.dwMaxSize = m_wCodecBufferSize;
    m_CodecMediaBuffer.pvBuffer = new BYTE[m_wCodecBufferSize];
    if( NULL == m_CodecMediaBuffer.pvBuffer )
    {
        OUTPUT_DEBUG_STRING( "CCommunicator::Inserted()...Not enough memory for Codec buffer!\n" );
        return E_OUTOFMEMORY;
    }

    // Feed the microphone with buffers; we enqueue all the buffers to the 
    // microphone which is going to fill them with data gradually
    // We don't enqueue buffers to the headphone yet; this is going to be done 
    // when buffers from the microphonestart to arrive
    MEDIA_PACKET_LIST *pTmpMBuff = m_pBeginMicrophoneQueuedBuffers;
    for( DWORD i = 0 ; i < m_wVoiceBuffersCount ; i++ )
    {
        QueueMicrophoneBuffer(pTmpMBuff);
        pTmpMBuff = pTmpMBuff->pNext;
    }


#ifdef _WRITE_LOG
    static int index = 0;
    char szFileName[30];
    //
    // Log File for the input PCM data from the microphone
    //
    sprintf(szFileName , "D:\\RawPCMData0%d.log" , index);
    OpenFile(szFileName , &m_hFileRawPCMData);
    
    //
    // Log File for the compressed signal
    //
    sprintf(szFileName , "D:\\Compressed0%d.log" , index);
    OpenFile(szFileName , &m_hFileCompressed);

    //
    // Log File for the decompressed signal
    //
    sprintf(szFileName , "D:\\Decompressed0%d.log" , index);
    OpenFile(szFileName , &m_hFileDecompressed);

    index++;

#endif

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Removed
// Desc: Called when the communicator is unplugged
//-----------------------------------------------------------------------------
void CCommunicator::Removed()
{
    CCommunicator::Cleanup();
    OUTPUT_DEBUG_STRING("Communicator removed!\n");
}




//-----------------------------------------------------------------------------
// Name: QueueMicrophoneBuffer
// Desc: Prepare a packet for the microphone and call Process on it
//-----------------------------------------------------------------------------
void CCommunicator::QueueMicrophoneBuffer(MEDIA_PACKET_LIST *pMediaPacketListEntry)
{
    XMEDIAPACKET *pMediaPacket = &(pMediaPacketListEntry->MediaPacket);

    //  Initialize the event 
    pMediaPacket->hCompletionEvent = m_hMicrophoneWorkEvent;

    //  Submit the buffer
    VERIFY_SUCCESS(m_pMicrophone->Process(NULL, pMediaPacket));
}




//-----------------------------------------------------------------------------
// Name: QueueHeadphoneBuffer 
// Desc: Prepare a packet for the headphone and call Process on it
//-----------------------------------------------------------------------------
void CCommunicator::QueueHeadphoneBuffer(MEDIA_PACKET_LIST *pMediaPacketListEntry)
{
    XMEDIAPACKET *pMediaPacket = &(pMediaPacketListEntry->MediaPacket);

    //  Initialize the event
    pMediaPacket->hCompletionEvent = m_hHeadphoneWorkEvent;

    //  Submit the buffer
    VERIFY_SUCCESS(m_pHeadphone->Process(pMediaPacket, NULL));

}




//-----------------------------------------------------------------------------
// Name: CompressMediaBuffer 
// Desc: Compress a PCM buffer and return the encoded buffer and a status on voice detected
//-----------------------------------------------------------------------------
HRESULT CCommunicator::CompressMediaBuffer(XMEDIAPACKET *pSrc, XMEDIAPACKET *pDst , BOOL *pvbVoiceDetected)
{
    HRESULT hr;
    DWORD dwSizeDst = 0; 

    assert( pDst->dwMaxSize == m_wCodecBufferSize );

    memset( pDst->pvBuffer, 0, pDst->dwMaxSize );

    pDst->pdwCompletedSize = &dwSizeDst;
    pDst->hCompletionEvent = NULL;

    // Encode the packet
    hr = m_pEncoder->Process( pSrc, pDst );

    assert( !FAILED(hr) );
    
#ifdef _WRITE_LOG
    WriteToLogFile(m_hFileCompressed , (BYTE*)pDst->pvBuffer , pDst->dwMaxSize);
#endif

    if( dwSizeDst < m_wCodecBufferSize )
    {
        *pvbVoiceDetected = FALSE;
    }
    else
    {
        *pvbVoiceDetected = TRUE;
    }

    pDst->pdwCompletedSize = NULL;

    return( hr );
}




//-----------------------------------------------------------------------------
// Name: OnMicrophoneInput 
// Desc: Called when a microphone event is set; take the buffer from the microphone 
//       and compress it, then sent it to the output
//-----------------------------------------------------------------------------
HRESULT CCommunicator::OnMicrophoneInput()
{
    HRESULT hr;
    BOOL bVoiceDetected = TRUE;
    
    if( NULL != m_pMicrophone )
    {
        // One microphone buffer is filled with data
        if(XMEDIAPACKET_STATUS_PENDING != m_pBeginMicrophoneQueuedBuffers->dwStatus)
        {
            if( SUCCEEDED( m_pBeginMicrophoneQueuedBuffers->dwStatus ) )
            {
#ifdef _WRITE_LOG
                WriteToLogFile(m_hFileRawPCMData,
                               (BYTE*)(m_pBeginMicrophoneQueuedBuffers->MediaPacket).pvBuffer,
                               m_wVoiceBufferSize );
#endif
                // 1. Compress the buffer buffer
                CompressMediaBuffer( &m_pBeginMicrophoneQueuedBuffers->MediaPacket,
                                     &m_CodecMediaBuffer, 
                                     &bVoiceDetected );
            
                // 2. Expose compressed data to any derived classes
                OnCompressedInput( &m_CodecMediaBuffer, bVoiceDetected );
            }

            if( XMEDIAPACKET_STATUS_FAILURE == m_pBeginMicrophoneQueuedBuffers->dwStatus )
            {
                hr = E_FAIL;
                return hr;
            }
            else
            {
                // 3. Submit the packet back to the microphone
                m_pBeginMicrophoneQueuedBuffers->dwStatus = (DWORD)XMEDIAPACKET_STATUS_PENDING;
                QueueMicrophoneBuffer(m_pBeginMicrophoneQueuedBuffers);
                m_pBeginMicrophoneQueuedBuffers = m_pBeginMicrophoneQueuedBuffers->pNext;
            }
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: QueuePCMForOutput
// Desc: Send a PCM buffer to the headphone and reuse the buffer
//-----------------------------------------------------------------------------
HRESULT CCommunicator::QueuePCMForOutput( XMEDIAPACKET *pOutput )
{
    // Validate input data
    assert(pOutput->dwMaxSize == m_wVoiceBufferSize);

#ifdef _WRITE_LOG
    WriteToLogFile(m_hFileDecompressed ,
                   (BYTE*)(m_pBeginMicrophoneQueuedBuffers->MediaPacket).pvBuffer ,
                   m_wVoiceBufferSize);
#endif

    // Send PCM data to the headphone
    memcpy(m_pBeginHeadphoneQueuedBuffers->MediaPacket.pvBuffer , pOutput->pvBuffer , pOutput->dwMaxSize);
    m_pBeginHeadphoneQueuedBuffers->MediaPacket.dwMaxSize = pOutput->dwMaxSize;
    QueueHeadphoneBuffer(m_pBeginHeadphoneQueuedBuffers);
    
    // Requeue the packet
    m_pBeginHeadphoneQueuedBuffers = m_pBeginHeadphoneQueuedBuffers->pNext;

    return S_OK;
}




//////////////////////////////////////////////////////////////////////////////
// Implementation of CVoiceChatCommunicator - Voice chat between xboxes
//////////////////////////////////////////////////////////////////////////////



//-----------------------------------------------------------------------------
// Name: CVoiceChatCommunicator
// Desc: Constructor
//-----------------------------------------------------------------------------
CVoiceChatCommunicator::CVoiceChatCommunicator( WORD   wHeadsetPort,
                                                HANDLE hEvtMicInput,
                                                WORD   wCodecTag,
                                                WORD   wVoicePort,
                                                WORD   wVoiceSamplingRate,
                                                WORD   wCodecSamplingRate,
                                                WORD   wVoiceBufferTime,
                                                WORD   wVoiceBufferCount ) :
    CCommunicator( wHeadsetPort,
                   hEvtMicInput,
                   wCodecTag,
                   wVoiceSamplingRate,
                   wCodecSamplingRate,
                   wVoiceBufferTime,
                   wVoiceBufferCount )
{
    m_VoiceQueueXMOList                 = NULL;
    m_nChattersCount                    = 0;
    m_fMixingBuffer                     = NULL;
    m_pResumePoint                      = NULL;
    m_nMixedPacketsCount                = 0;
    m_VoiceSock                         = INVALID_SOCKET;
    m_wVoicePort                        = wVoicePort;
    m_MixedSoundMediaBuffer.pvBuffer    = NULL;
    m_NetworkMediaBuffer.pvBuffer       = NULL;
}




//-----------------------------------------------------------------------------
// Name: ~CVoiceChatCommunicator
// Desc: Destructor
//-----------------------------------------------------------------------------
CVoiceChatCommunicator::~CVoiceChatCommunicator()
{
    CVoiceChatCommunicator::Cleanup();
}




//-----------------------------------------------------------------------------
// Name: Cleanup
// Desc: Called at destruction time or when the communicator is removed
//-----------------------------------------------------------------------------
void CVoiceChatCommunicator::Cleanup()
{
    DeleteQueueXMOList();

    if( NULL != m_MixedSoundMediaBuffer.pvBuffer )
    {
        delete [] m_MixedSoundMediaBuffer.pvBuffer;
        m_MixedSoundMediaBuffer.pvBuffer = NULL;
    }

    if( NULL != m_NetworkMediaBuffer.pvBuffer )
    {
        delete [] m_NetworkMediaBuffer.pvBuffer;
        m_NetworkMediaBuffer.pvBuffer = NULL;
    }

    if( NULL != m_fMixingBuffer )
    {
        delete [] m_fMixingBuffer;
        m_fMixingBuffer = NULL;
    }


    if( INVALID_SOCKET != m_VoiceSock )
    {
        closesocket( m_VoiceSock );
        m_VoiceSock = INVALID_SOCKET;
    }
}




//-----------------------------------------------------------------------------
// Name: Inserted
// Desc: Called when a communicator is newly detected in this slot, 
//          performs all initialization
//-----------------------------------------------------------------------------
HRESULT CVoiceChatCommunicator::Inserted()
{
    HRESULT hr;
    
    // Base class initialization
    hr = CCommunicator::Inserted();
    if( FAILED( hr ) )
    {
        OUTPUT_DEBUG_STRING( "CVoiceChatCommunicator::Inserted() - Base class Inserted() failed!\n" );
        return hr;
    }

    // Create the buffer used by the queue output
    memset( &m_MixedSoundMediaBuffer, 0, sizeof( XMEDIAPACKET ) );
    m_MixedSoundMediaBuffer.dwMaxSize = m_wVoiceBufferSize;
    m_MixedSoundMediaBuffer.pvBuffer = new BYTE[ m_wVoiceBufferSize ];

    if( NULL == m_MixedSoundMediaBuffer.pvBuffer )
    {
        DebugPrint("CVoiceChatCommunicator::Inserted()...Not enough memory for queue output buffer!\n");
        return E_OUTOFMEMORY;
    }
    else
    {
        // This buffer may be used for first output for headphone if nothing comes from network,
        // so we initialize it with silence
        memset( m_MixedSoundMediaBuffer.pvBuffer, 0, m_wVoiceBufferSize );
    }

    // Create the buffer used by the network input
    memset(&m_NetworkMediaBuffer, 0, sizeof(XMEDIAPACKET));
    m_NetworkMediaBuffer.dwMaxSize = m_wCodecBufferSize;
    m_NetworkMediaBuffer.pvBuffer = new BYTE[m_wCodecBufferSize];

    if( NULL == m_NetworkMediaBuffer.pvBuffer )
    {
        OUTPUT_DEBUG_STRING( "CVoiceChatCommunicator::Inserted()...Not enough memory for network input buffer!\n" );
        return E_OUTOFMEMORY;
    }

    // Create the buffer used for mixing sounds - size is /sizeof(short) because we are mixing 16 bits/sample
    m_wMixingBufferSize = m_wVoiceBufferSize / sizeof(short);
    m_fMixingBuffer = new float[m_wMixingBufferSize];

    if( NULL ==  m_fMixingBuffer)
    {
        OUTPUT_DEBUG_STRING("CVoiceChatCommunicator::Inserted()...Not enough memory for mixing buffer!\n");
        return E_OUTOFMEMORY;
    }

    // Network stuff initialization
    m_VoiceSock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if( INVALID_SOCKET ==  m_VoiceSock )
    {
        hr = WSAGetLastError();
        OUTPUT_DEBUG_STRING( "CVoiceChatCommunicator::Inserted() - Voice socket open failed\n" );
        return hr;
    }

    SOCKADDR_IN voiceAddr;
    voiceAddr.sin_family = AF_INET;
    voiceAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    voiceAddr.sin_port = htons( m_wVoicePort );

    int iResult = bind( m_VoiceSock, (const sockaddr*)(&voiceAddr), sizeof( SOCKADDR_IN ) );
    if(iResult == SOCKET_ERROR)
    {
       hr = WSAGetLastError();
       OUTPUT_DEBUG_STRING( "CVoiceChatCommunicator::Inserted()...Voice socket binding failed\n" );
       return hr;
    }

    DWORD dwNonBlocking = 1;
    iResult = ioctlsocket(m_VoiceSock , FIONBIO, &dwNonBlocking );
    if(iResult == SOCKET_ERROR)
    {
       hr = WSAGetLastError();
       OUTPUT_DEBUG_STRING( "CVoiceChatCommunicator::Inserted() - ioctlsocket failed\n" );
       return hr;
    }

    // Output is requested from the queuing XMO only if m_IntervalForOutput time has elapsed
    m_IntervalForOutput = m_wVoiceBufferTime / 2;
    m_LastOutputTimeStamp = GetTickCount();


    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Removed
// Desc: Called when the communicator is unplugged
//-----------------------------------------------------------------------------
void CVoiceChatCommunicator::Removed()
{
    CVoiceChatCommunicator::Cleanup();

    CCommunicator::Removed();
}




//-----------------------------------------------------------------------------
// Name: OnCompressedInput
// Desc: Pure function in CCommunicator, implemented here.
//       Send the encoded voice packet when voice is detected in it.
//-----------------------------------------------------------------------------
HRESULT CVoiceChatCommunicator::OnCompressedInput( XMEDIAPACKET *pInput , BOOL bVoiceDetected )
{
    if( bVoiceDetected )
    {
        SendVoicePacket(pInput);
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CheckForOutputData
// Desc: Pure function in CCommunicator, implemented here.
//       Read the network and insert any incoming buffer in its corresponding
//       queueXMO. If enough time elapses, process the existing queues.
//       If valid PCM buffer is returned, send it to the headphone.
//-----------------------------------------------------------------------------
HRESULT CVoiceChatCommunicator::CheckForOutputData()
{
    HRESULT hr = E_FAIL; 

    if( NULL != m_pMicrophone )
    {
        ReadNetwork();

        // Call for output only if the interval elapsed and the oldest headphone buffer has been processed
        if((GetTickCount() - m_LastOutputTimeStamp >= m_IntervalForOutput) &&
            (m_pBeginHeadphoneQueuedBuffers->dwStatus == XMEDIAPACKET_STATUS_SUCCESS))
        {         
            hr = ProcessQueues(); 

            m_LastOutputTimeStamp = GetTickCount();
        }
        

        if( SUCCEEDED( hr ) )
        {
            // Send it to the headphone
            QueuePCMForOutput(&m_MixedSoundMediaBuffer);
        }
    }

    return hr;


}

//-----------------------------------------------------------------------------
// Name: SendVoicePacket
// Desc: Send a voice packet to all the chatters
//-----------------------------------------------------------------------------
void CVoiceChatCommunicator::SendVoicePacket( XMEDIAPACKET *pMediaPacket )
{
    int nBytes = 0;
    QUEUEXMO_PER_CHATTER *pTmp = NULL;

    // Send the voice packet to each remote chatter
    for( pTmp = m_VoiceQueueXMOList ; NULL != pTmp ; pTmp = pTmp->m_pNext)
    {
        nBytes = sendto( m_VoiceSock , (const char*)pMediaPacket->pvBuffer, pMediaPacket->dwMaxSize , 
                         0, (const sockaddr*)(&(pTmp->m_SockAddrDest)), sizeof( SOCKADDR_IN ) );

        assert( nBytes == m_wCodecBufferSize );
    }
}




//-----------------------------------------------------------------------------
// Name: ReadNetwork
// Desc: Read the network and insert any incoming buffer in its corresponding
//       queueXMO.
//-----------------------------------------------------------------------------
void CVoiceChatCommunicator::ReadNetwork()
{
    HRESULT hr = S_OK;
    WORD i;
    SOCKADDR_IN saFromIn;
    BOOL bReadOK = TRUE;

    for( i = 0 ; (i < m_nChattersCount ) && (SUCCEEDED(hr)) && (bReadOK) ; i++ )
    {
        // See if a voice message is waiting for us
        int iSize = sizeof( SOCKADDR_IN );

        memset(m_NetworkMediaBuffer.pvBuffer , 0 , m_NetworkMediaBuffer.dwMaxSize);
        int iResult = recvfrom( m_VoiceSock , (char*)m_NetworkMediaBuffer.pvBuffer, m_NetworkMediaBuffer.dwMaxSize, 
                            0, (sockaddr*)(&saFromIn), &iSize );
    

        if( iResult != SOCKET_ERROR && iResult > 0 )
        {
            // TBD: analyze iResult and see if a whole packet has come....
            QUEUEXMO_PER_CHATTER *pQueue = FindInQueueXMOList(saFromIn);

            if( NULL != pQueue )
            {
                hr = pQueue->m_pVoiceQueueXMO->Process(&m_NetworkMediaBuffer , NULL);
            }
        }
        else
        {
            bReadOK = FALSE;
        }
    }
}




//-----------------------------------------------------------------------------
// Name: ProcessQueues
// Desc: Ask all the queues for output and if more than one, mix the PCM buffers 
//       that have voice.
//-----------------------------------------------------------------------------
HRESULT CVoiceChatCommunicator::ProcessQueues()
{
    HRESULT hr = S_OK;

    // Get output from the queues and mix the sound if necessary
    switch( m_nChattersCount )
    {
    case 0:
        hr = E_FAIL;
        break;

    case 1:
        // With only one remote chatter, we don't have to mix output
        hr = (m_VoiceQueueXMOList->m_pVoiceQueueXMO)->Process(NULL , &m_MixedSoundMediaBuffer);
        if( SUCCEEDED( hr ) )
        {
            m_VoiceQueueXMOList->m_bProvidedOutput = TRUE;
        }
        break;

    default:
        // Default case with multiple chatters means we have to mix output
        QUEUEXMO_PER_CHATTER *pTmp;
        PCM_INFO outputInfo;
        if( NULL == m_pResumePoint )
        {
            pTmp = m_VoiceQueueXMOList;
            memset(m_fMixingBuffer , 0 , m_wMixingBufferSize * sizeof(float) );
            m_nMixedPacketsCount = 0;
        }
        else
        {
            pTmp = m_pResumePoint;
        }
        
        for( ; NULL != pTmp; pTmp = pTmp->m_pNext )
        { 
            hr = (pTmp->m_pVoiceQueueXMO)->Process(NULL , &m_MixedSoundMediaBuffer);
            if( SUCCEEDED(hr) )
            {
                m_pResumePoint = NULL;

                // We'll mix in only the packets that have voice inside
                (pTmp->m_pVoiceQueueXMO)->GetLastOutputInfo(&outputInfo);
                if( outputInfo.nMaxPower > 0  )
                {
                    pTmp->m_bProvidedOutput = TRUE;
                    SumUpPacket(&m_MixedSoundMediaBuffer);
                    m_nMixedPacketsCount++;
                }
            }
            else
            {
                // Syncronization
                if(pTmp->m_bProvidedOutput)
                {
                    m_pResumePoint = pTmp;
                    break;
                }
            }
        }

        if( NULL != m_pResumePoint )
        {
            hr = E_FAIL;
        }
        else
        {
            // If m_nMixedPacketsCount == 0 we'll return the last of the queues' output
            // (it's silence, anyway)
            if( 0 != m_nMixedPacketsCount )
            {
                short *pBuffer = (short*)m_MixedSoundMediaBuffer.pvBuffer;
                for( WORD k = 0 ; k < m_wMixingBufferSize ; k++ )
                {
                    pBuffer[k] = (short)(m_fMixingBuffer[k] / m_nMixedPacketsCount);
                }
            }

            m_pResumePoint = NULL;
            hr = S_OK;
        }
    }
    
    return hr;
}




//-----------------------------------------------------------------------------
// Name: SumUpPacket
// Desc: Mixing 
//-----------------------------------------------------------------------------
void CVoiceChatCommunicator::SumUpPacket( XMEDIAPACKET *pMediaPacket )
{
    // This function is adding the samples in the mixing buffer 
    assert(pMediaPacket->dwMaxSize == m_wVoiceBufferSize);
    short *pBuffer = (short*)pMediaPacket->pvBuffer;

    for( WORD k = 0 ; k <  m_wMixingBufferSize ; k++)
    {
        m_fMixingBuffer[k] += (float)pBuffer[k];
    }
}




//-----------------------------------------------------------------------------
// Name: RemoveChatters
// Desc: Called when one of the xboxes left the chat(if it has multiple communicators
//       plugged in, remove all of them from the chat)
//-----------------------------------------------------------------------------
HRESULT CVoiceChatCommunicator::RemoveChatters(const IN_ADDR &inAddr)
{
    QUEUEXMO_PER_CHATTER *pTmp = m_VoiceQueueXMOList;
    QUEUEXMO_PER_CHATTER *pPrev = NULL;

    while( NULL != pTmp )
    {
        if( pTmp->IsSameXbox(inAddr) )
        {
            if( NULL == pPrev )
            {
                m_VoiceQueueXMOList = m_VoiceQueueXMOList->m_pNext;
                delete pTmp;
                pTmp = m_VoiceQueueXMOList;
            }
            else
            {
                pPrev->m_pNext = pTmp->m_pNext;
                delete pTmp;
                pTmp = pPrev->m_pNext;
            }
            
            m_nChattersCount--;
        }
        else
        {
            pPrev = pTmp;
            pTmp = pTmp->m_pNext;
        }
    }

    return (S_OK);
}




//-----------------------------------------------------------------------------
// Name: UpdateChatList
// Desc: Add one chatter, or, if no VoicePort provided and delete requested,
//       remove all the chatters from that IP address
//-----------------------------------------------------------------------------
HRESULT CVoiceChatCommunicator::UpdateChatList( BOOL bAdd, 
                                                const IN_ADDR &inAddr, 
                                                WORD wVoicePort )
{
    HRESULT hr = S_OK;
    QUEUEXMO_PER_CHATTER *pQueue = NULL;

    if( NULL != m_pMicrophone )
    {
        if( 0 == wVoicePort )
        {
            if( !bAdd )
            {
                RemoveChatters(inAddr);
                return (hr); 
            }
        }
        
        pQueue = FindInQueueXMOList(inAddr , wVoicePort);
        
        if( bAdd )
        {
            if(NULL == pQueue)
            {
                // Voice queueing initialization
                QUEUE_XMO_CONFIG config;
                QUEUEXMO_PER_CHATTER queuePerPlayer;
    
                // Set up the queue configuration
                memset(&config , 0 , sizeof(config));
                config.cbSize = sizeof(config);
                config.wMsOfDataPerPacket = m_wVoiceBufferTime;
                config.wInitialHighWaterMark = 2 * m_wVoiceBufferTime;
                config.wMinDelay = 2 * m_wVoiceBufferTime;
                config.wMaxDelay = 2 * m_wVoiceBuffersCount * m_wVoiceBufferTime;
                config.dwCodecTag = (DWORD)m_wCodecTag;

                hr = XVoiceQueueCreateMediaObject(&config , &(queuePerPlayer.m_pVoiceQueueXMO));
                if( FAILED( hr ) )
                {
                    OUTPUT_DEBUG_STRING( "CVoiceChatCommunicator::UpdateChatList() - Add player failed!\n" );
                    return hr;
                }

                queuePerPlayer.m_SockAddrDest.sin_family = AF_INET;
                queuePerPlayer.m_SockAddrDest.sin_addr = inAddr;
                queuePerPlayer.m_SockAddrDest.sin_port = htons(wVoicePort);

                AddToQueueXMOList(queuePerPlayer);
                queuePerPlayer.m_pVoiceQueueXMO = NULL;
            }
        }
        else 
        {
            if( NULL != pQueue)
            {
                DeleteFromQueueXMOList(pQueue);
            }
        }
    }

    return S_OK;
}


//
// The methods for QueueXMO list handling
//

//-----------------------------------------------------------------------------
// Name: AddToQueueXMOList
// Desc: Insert one queue in the queue list
//-----------------------------------------------------------------------------
void CVoiceChatCommunicator::AddToQueueXMOList(const QUEUEXMO_PER_CHATTER& queue)
{
    QUEUEXMO_PER_CHATTER *pTmp = NULL;
    QUEUEXMO_PER_CHATTER *pAux = NULL;
    
    if( NULL == m_VoiceQueueXMOList )
    {
        pAux = m_VoiceQueueXMOList = new QUEUEXMO_PER_CHATTER(queue);
    }
    else
    {
        for(pTmp = m_VoiceQueueXMOList ; NULL != pTmp->m_pNext ; pTmp = pTmp->m_pNext){}
        pAux = pTmp->m_pNext = new QUEUEXMO_PER_CHATTER(queue);

    }
    if( NULL != pAux )
    {
        m_nChattersCount++;
    }
}

//-----------------------------------------------------------------------------
// Name: FindInQueueXMOList
// Desc: Find a particular queue in the queue list
//-----------------------------------------------------------------------------
QUEUEXMO_PER_CHATTER* CVoiceChatCommunicator::FindInQueueXMOList( const IN_ADDR &inAddr , WORD wVoicePort)
{
    QUEUEXMO_PER_CHATTER *pTmp = NULL;

    for( pTmp = m_VoiceQueueXMOList ; NULL != pTmp ; pTmp = pTmp->m_pNext)
    {
        if( pTmp->IsSameCommunicator(inAddr , wVoicePort) )
        {
            break;
        }
    }

    return pTmp;
}

//-----------------------------------------------------------------------------
// Name: FindInQueueXMOList
// Desc: Find a particular queue in the queue list
//-----------------------------------------------------------------------------
QUEUEXMO_PER_CHATTER* CVoiceChatCommunicator::FindInQueueXMOList( const SOCKADDR_IN &addr)
{
    QUEUEXMO_PER_CHATTER *pTmp = NULL;

    for( pTmp = m_VoiceQueueXMOList ; NULL != pTmp ; pTmp = pTmp->m_pNext)
    {
        if( pTmp->IsSameCommunicator(addr) )
        {
            break;
        }
    }

    return pTmp;
}

//-----------------------------------------------------------------------------
// Name: DeleteFromQueueXMOList
// Desc: Delete a queue from the queue list
//-----------------------------------------------------------------------------
void CVoiceChatCommunicator::DeleteFromQueueXMOList(QUEUEXMO_PER_CHATTER *pQueue)
{

    QUEUEXMO_PER_CHATTER *pTmp = m_VoiceQueueXMOList;
    QUEUEXMO_PER_CHATTER *pPrev = NULL;

    DebugPrint("Delete from queue list...\n");

    for(  ; NULL != pTmp ; pPrev = pTmp , pTmp = pTmp->m_pNext )
    {
        if( pTmp == pQueue)
        {
            if( NULL == pPrev )
            {
                m_VoiceQueueXMOList = m_VoiceQueueXMOList->m_pNext;
            }
            else
            {
                pPrev->m_pNext = pTmp->m_pNext;
            }

            if( pTmp == m_pResumePoint )
            {
                //
                // If the resume point for mixing is deleted...
                //
                m_pResumePoint = pTmp->m_pNext;
            }

            delete pQueue;
            m_nChattersCount--;

            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Name: DeleteQueueXMOList
// Desc: Delete the entire queue list
//-----------------------------------------------------------------------------
void CVoiceChatCommunicator::DeleteQueueXMOList()
{
    
    QUEUEXMO_PER_CHATTER *pTmp = NULL;

    DebugPrint("Delete queue list...\n");

    while(NULL != m_VoiceQueueXMOList)
    {
        pTmp = m_VoiceQueueXMOList;
        m_VoiceQueueXMOList = m_VoiceQueueXMOList->m_pNext;
        delete pTmp;
    }
    m_VoiceQueueXMOList = NULL;
    m_nChattersCount = 0;

}



//////////////////////////////////////////////////////////////////////////////
//
// Implementation of CCommunicatorMgr that tracks multiple Xbox Communicators
//
//////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------
// Name: CCommunicatorMgr
// Desc: Constructor
//-----------------------------------------------------------------------------
CCommunicatorMgr::CCommunicatorMgr()
{   
}




//-----------------------------------------------------------------------------
// Name: ~CCommunicatorMgr
// Desc: Destructor
//-----------------------------------------------------------------------------
CCommunicatorMgr::~CCommunicatorMgr()
{
    for(BYTE k = 0 ; k < COMMUNICATOR_COUNT  ; k++)
    {
        if( NULL != m_pVoiceUnits[k] )
        {
            delete m_pVoiceUnits[k];
            m_pVoiceUnits[k] = NULL;
        }
    }

    if (m_hCommunicatorWorkEvent) 
    {
        CloseHandle(m_hCommunicatorWorkEvent);
        m_hCommunicatorWorkEvent = INVALID_HANDLE_VALUE;
    }
}




//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Called when the voice chat is initiated on this xbox
//-----------------------------------------------------------------------------
HRESULT CCommunicatorMgr::Initialize( PlayerList& playerList, 
                                      WORD wCodecTag, 
                                      MsgVoicePort *msgCommunicatorStatus )
{
    HRESULT hr = S_OK;
    WORD i , j;

    assert(NULL != msgCommunicatorStatus);

    // Create the work event for the voice units
    m_hCommunicatorWorkEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if( NULL ==  m_hCommunicatorWorkEvent)
    {
        return E_FAIL;
    }

    // Create one communicator for each port
    for(i = 0 ; i < COMMUNICATOR_COUNT ; i++)
    {
        m_pVoiceUnits[i] = new CVoiceChatCommunicator(i , m_hCommunicatorWorkEvent , wCodecTag , VOICE_PORT  + i );
        if( NULL ==  m_pVoiceUnits[i])
        {
            return E_OUTOFMEMORY;
        }
    }        
    
    // Verify the initial state of the voice devices
    hr = CheckInitialState();

    // Tell the communicator manager which slots are active
    BOOL bFirst = TRUE;
    for( i = 0; i < COMMUNICATOR_COUNT; i++ )
    {
        if( m_pVoiceUnits[i]->IsCommunicatorActive() )
        {
            if( bFirst )
            {
                msgCommunicatorStatus[i].action = ADD_AND_RESPOND;
                bFirst = FALSE;
            }
            else
            {
                msgCommunicatorStatus[i].action = ADD_PORT;
            }
            msgCommunicatorStatus[i].wVoicePort = m_pVoiceUnits[i]->GetVoicePort();
        }
    }
        
    // Initialize queues for the players on the same xbox
    for( i = 0 ; i < COMMUNICATOR_COUNT ; i++ )
    {
        if( m_pVoiceUnits[i]->IsCommunicatorActive() )
        {
            for( j = 0; j <  COMMUNICATOR_COUNT; j++ )
            {
                if( i != j )
                {
                    IN_ADDR inAddr;
                    inAddr.s_addr = htonl(INADDR_LOOPBACK);
                    hr = m_pVoiceUnits[j]->UpdateChatList( TRUE  , inAddr , m_pVoiceUnits[i]->GetVoicePort() );
                    if( FAILED( hr ) )
                    {
                        OUTPUT_DEBUG_STRING("CVoiceChatCommunicator::Initialize() - Failed to initialize queues!\n");
                        break;
                    }
                }
            }
         }
    }

    return hr;
}




//-----------------------------------------------------------------------------
// Name: Process 
// Desc: The engine of the whole voice chat, called in the main infinite loop
//-----------------------------------------------------------------------------
HRESULT CCommunicatorMgr::Process(MsgVoicePort *msgCommunicatorStatus)
{
    BYTE i;
    HRESULT hr;

    assert(NULL != msgCommunicatorStatus);

    DWORD dwReason = WaitForSingleObject(m_hCommunicatorWorkEvent ,  0);


    // One of the voice units has microphone input
    if( WAIT_OBJECT_0 == dwReason )
    {
        for( i = 0 ; i <  COMMUNICATOR_COUNT ; i++ )
        {
            hr = m_pVoiceUnits[i]->OnMicrophoneInput();
            if(FAILED(hr))
            {
                OUTPUT_DEBUG_STRING( "CCommunicatorMgr::Process() - Error OnMicrophoneInput\n" );
                return (hr);
            }
        }
    }
            
    // Check for output data even if no microphone input(read the network for example)
    for( i = 0 ; i <  COMMUNICATOR_COUNT ; i++ )
    {
        m_pVoiceUnits[i]->CheckForOutputData();
    }
    
    //  Always check for hot-plugs
    return CheckForHotPlugs(msgCommunicatorStatus);
}




//-----------------------------------------------------------------------------
// Name: CheckInitialState
// Desc: Get the initial state of the devices plugged in, called in Initialize()
//-----------------------------------------------------------------------------
HRESULT CCommunicatorMgr::CheckInitialState()
{
    HRESULT hr = S_OK;
    
    //  Get the initial connected state of the devices
    m_dwConnectedCommunicators = 0;
    m_dwMicrophoneState = XGetDevices( XDEVICE_TYPE_VOICE_MICROPHONE );
    m_dwHeadphoneState  = XGetDevices( XDEVICE_TYPE_VOICE_HEADPHONE );

    for( INT i = 0; i < COMMUNICATOR_COUNT; i++ )
    {
        if( m_dwMicrophoneState & ( 1 << i ) &&
            m_dwHeadphoneState  & ( 1 << i ) )
        {
            if(!SUCCEEDED(m_pVoiceUnits[i]->Inserted()))
            {
                OUTPUT_DEBUG_STRING( "CCommunicatorMgr::CheckInitialState() - Voice unit initialization failed!\n" );
                return E_FAIL;
            }
            m_dwConnectedCommunicators |= ( 1 << i );
        }
    }
    
    return hr;
}




//-----------------------------------------------------------------------------
// Name: CheckForHotPlugs
// Desc: Called in Process to check if new inserts/removes occured
//-----------------------------------------------------------------------------
HRESULT CCommunicatorMgr::CheckForHotPlugs(MsgVoicePort *msgCommunicatorStatus)
{
    DWORD dwMicrophoneInsertions;
    DWORD dwMicrophoneRemovals;
    DWORD dwHeadphoneInsertions;
    DWORD dwHeadphoneRemovals;

    HRESULT hr;
    IN_ADDR inAddr;

    inAddr.s_addr = htonl(INADDR_LOOPBACK);

    // Must call XGetDevice changes to track possible removal and insertion
    // in one frame
    XGetDeviceChanges( XDEVICE_TYPE_VOICE_MICROPHONE,
                       &dwMicrophoneInsertions,
                       &dwMicrophoneRemovals );
    XGetDeviceChanges( XDEVICE_TYPE_VOICE_HEADPHONE,
                       &dwHeadphoneInsertions,
                       &dwHeadphoneRemovals );

    // Update state for removals
    m_dwMicrophoneState &= ~( dwMicrophoneRemovals );
    m_dwHeadphoneState  &= ~( dwHeadphoneRemovals );

    // Then update state for new insertions
    m_dwMicrophoneState |= ( dwMicrophoneInsertions );
    m_dwHeadphoneState  |= ( dwHeadphoneInsertions );

    // Check each communicator for possible removal and
    // then for insertion
    for(BYTE i = 0 ; i < COMMUNICATOR_COUNT ; i++ )
    {
        // If either the microphone or the headphone was
        // removed since last call, remove the communicator
        if( m_dwConnectedCommunicators & ( 1 << i ) &&
            ( ( dwMicrophoneRemovals & ( 1 << i ) ) ||
              ( dwHeadphoneRemovals  & ( 1 << i ) ) ) )
        {
            m_dwConnectedCommunicators &= ~( 1 << i );
            m_pVoiceUnits[i]->Removed();

            msgCommunicatorStatus[i].action = DELETE_PORT;
            hr = UpdateChatList( FALSE  , inAddr , msgCommunicatorStatus[i].wVoicePort );
            if( FAILED( hr ) )
                return hr;
        }

        // If both microphone and headphone are present, and
        // we didn't have a communicator here last frame,
        // register the insertion
        if( ( m_dwMicrophoneState & ( 1 << i ) ) &&
            ( m_dwHeadphoneState  & ( 1 << i ) ) &&
            !( m_dwConnectedCommunicators & ( 1 << i ) ) )
        {
            hr = m_pVoiceUnits[i]->Inserted();
            if( FAILED( hr ) )
                return hr;

            msgCommunicatorStatus[i].action = ADD_AND_RESPOND;
            msgCommunicatorStatus[i].wVoicePort = m_pVoiceUnits[i]->GetVoicePort();

            for( BYTE k = 0 ; k < COMMUNICATOR_COUNT ; k++)
            {
                if( k != i )
                {
                    if( m_pVoiceUnits[k]->IsCommunicatorActive() )
                    {
                        m_pVoiceUnits[k]->UpdateChatList(TRUE , inAddr , m_pVoiceUnits[i]->GetVoicePort());
                        m_pVoiceUnits[i]->UpdateChatList(TRUE , inAddr , m_pVoiceUnits[k]->GetVoicePort());
                    }
                }
            }

            // Note that the communicator has been added
            m_dwConnectedCommunicators |= ( 1 << i );
        }
    } 
         
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateChatList
// Desc: Tell the communicators to perform this update
//-----------------------------------------------------------------------------
HRESULT CCommunicatorMgr::UpdateChatList(BOOL bAdd , const IN_ADDR &inAddr , WORD wVoicePort)
{
    HRESULT hr = S_OK;

    for( BYTE i = 0 ; i <  COMMUNICATOR_COUNT ; i++ )
    {
        hr = m_pVoiceUnits[i]->UpdateChatList(bAdd , inAddr , wVoicePort);
    }

    return (hr);
    
}




//////////////////////////////////////////////////////////////////////////////
//
// Debug help functions implementation
//
//////////////////////////////////////////////////////////////////////////////
#ifdef _WRITE_LOG

void OpenFile(const char *szFileName, HANDLE *phFile)
{
    *phFile = CreateFile(szFileName,
                      GENERIC_WRITE,                // open for writing 
                      0,                            // do not share 
                      NULL,                         // no security 
                      CREATE_ALWAYS,                // overwrite existing 
                      0,                            // normal file 
                      NULL);                        // no attr. template 

    
    if (*phFile == INVALID_HANDLE_VALUE) 
    { 
        DWORD dwErr = GetLastError();
        DebugPrint("Error: %d.\n" , dwErr);  // process error 
    } 
}

void CloseFile(HANDLE *phFile)
{
    if( NULL != *phFile )
    {
        // Make sure the message makes it to the disk
        FlushFileBuffers( *phFile );
        CloseHandle(*phFile); 
        *phFile = NULL;
    }
}

void WriteToLogFile(HANDLE hFile, BYTE *pbBuffer, DWORD dwSize)
{
DWORD dwBytesWritten = 0;

    WriteFile( hFile,                     // handle to output file
               pbBuffer,                  // data buffer
               dwSize,                    // number of bytes to write
               &dwBytesWritten ,
               NULL);

    assert(dwSize == dwBytesWritten);

}
#endif

