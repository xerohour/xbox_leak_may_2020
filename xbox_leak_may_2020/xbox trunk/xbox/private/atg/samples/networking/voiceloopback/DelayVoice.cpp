//-----------------------------------------------------------------------------
// File: DelayVoice.cpp
//
// Hist: 08.08.01 - New for Aug M1 release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "DelayVoice.h"

#define DSOUND_INCLUDE_XHAWK


//////////////////////////////////////////////////////////////////////////////
//
// Implementation of CDelayVoiceUnit
//
//////////////////////////////////////////////////////////////////////////////

CDelayVoiceUnit::CDelayVoiceUnit():
        m_pMicrophone(NULL),
        m_pHeadphone(NULL),
        m_dwPortNumber(0),
        m_hMicrophoneWorkEvent(NULL),
        m_hHeadphoneWorkEvent(NULL),
        m_pBeginMicrophoneQueuedBuffers(NULL),
        m_pBeginHeadphoneQueuedBuffers(NULL),
        m_fPreBuffering(FALSE),
        m_dwPreBufferRegions(0)
{}

void CDelayVoiceUnit::Cleanup()
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

    
    if (m_hMicrophoneWorkEvent) 
    {
        CloseHandle(m_hMicrophoneWorkEvent);
        m_hMicrophoneWorkEvent = NULL;
    }

    if (m_hHeadphoneWorkEvent) 
    {
        CloseHandle(m_hHeadphoneWorkEvent);
        m_hHeadphoneWorkEvent = NULL;
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
}

void CDelayVoiceUnit::DeleteQueue(MEDIA_PACKET_LIST *pQueue)
{
    assert(pQueue->pNext != pQueue);

    MEDIA_PACKET_LIST *pTmpBuff = pQueue;

    pQueue = pQueue->pNext;
    pTmpBuff->pNext = NULL; //Break queue

    while(NULL != pQueue)
    {
        pTmpBuff = pQueue->pNext;
        delete pQueue;
        pQueue = pTmpBuff;
    }
}

CDelayVoiceUnit::~CDelayVoiceUnit()
{
    Cleanup();
}

void CDelayVoiceUnit::CreateQueue(MEDIA_PACKET_LIST **pQueue , WORD wSize , DWORD dwStatus)
{
    MEDIA_PACKET_LIST *pTmp = NULL;
    MEDIA_PACKET_LIST *pHead = NULL;
    
    
    //
    // Create linked-list
    //
    for( int i = 0 ; i <  wSize ; i++ )
    {
        if( pTmp )
        {
            pTmp->pNext = new MEDIA_PACKET_LIST(dwStatus);
            pTmp = pTmp->pNext;
        }
        else
        {
            pTmp = new MEDIA_PACKET_LIST(dwStatus);
            pHead = pTmp;
        }
        if( NULL == pTmp )
        {
            break;
        }
    }

    if(NULL == pTmp)
    {
        //
        // Something went wrong, release allocated memory
        //
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
        //
        // Close queue
        //
        pTmp->pNext = pHead;
        *pQueue = pHead;
    }
    
}

void CDelayVoiceUnit::Init(DWORD dwPortNumber)
{
    m_dwPortNumber = dwPortNumber;
}

void CDelayVoiceUnit::Process()
{
    DWORD dwReason;
    HRESULT hr;

        if( NULL == m_hMicrophoneWorkEvent )
        {
            return; // No Xbox Communicator inserted
        }

        for( ;; )
        {
            dwReason = WaitForSingleObject( m_hMicrophoneWorkEvent, 16);

            switch (dwReason)
            {
            case WAIT_OBJECT_0://Microphone
                hr = OnMicrophoneReadComplete();
                if(FAILED(hr))
                {
                    DebugPrint("Failure OnMicrophoneReadComplete = %d\n" , hr);
                    return;
                }
                else
                {
                    PlayToHeadphone();  
                }
                break;
            default: //Timeout
                return;
            }
        
        }
}

void CDelayVoiceUnit::SetSockAddr(const IN_ADDR &addrPlayer)
{
}

HRESULT CDelayVoiceUnit::Inserted()
{
    HRESULT         hr;
    WAVEFORMATEX    wfxFormat;
    
    DebugPrint("CDelayVoiceUnit::Inserted()...Xbox Communicator Inserted\n");

    //
    // The headset initialization
    //
    wfxFormat.wFormatTag = WAVE_FORMAT_PCM;
    wfxFormat.nChannels = 1;
    wfxFormat.nSamplesPerSec = VOICE_SAMPLING_RATE;
    wfxFormat.nAvgBytesPerSec = 2 * VOICE_SAMPLING_RATE;
    wfxFormat.nBlockAlign = 2;
    wfxFormat.wBitsPerSample = 16;
    wfxFormat.cbSize = 0;

    //
    //  Create a microphone and a headphone
    //
    DebugPrint("CDelayVoiceUnit::Inserted()...Creating microphone...\n");
    hr = XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_MICROPHONE, m_dwPortNumber , XVOICE_TOTAL_REGIONS,
                                 &wfxFormat, &m_pMicrophone);

    if( FAILED( hr ) )
    {
        DebugPrint("CDelayVoiceUnit::Inserted()...Microphone failed with %x", hr);
        return hr;
    }

    DebugPrint("CDelayVoiceUnit::Inserted()...Creating headphone...\n");
    hr = XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_HEADPHONE, m_dwPortNumber , XVOICE_TOTAL_REGIONS,
                                 &wfxFormat, &m_pHeadphone);

    if( FAILED( hr ) )
    {
        DebugPrint("CDelayVoiceUnit::Inserted()...Headphone failed with %x", hr);
        m_pMicrophone->Release();
        m_pMicrophone = NULL;
        return hr;
    }

    //
    // Other member initializing...
    //
    m_hMicrophoneWorkEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hHeadphoneWorkEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    
    m_fPreBuffering = TRUE;
    m_dwPreBufferRegions = 0;

/*#ifdef _DEBUG
    //
    // Log File for the converted signal 16kHz->8kHz
    //
    OpenFile("D:\\RawPCMData.log" , &m_hFileRawPCMData);
    
    //
    // Log File for the compressed signal
    //
    OpenFile("D:\\Compressed.log" , &m_hFileCompressed);

    //
    // Log File for the decompressed signal
    //
    OpenFile("D:\\Decompressed.log" , &m_hFileDecompressed);
#endif*/
    //
    // Create the buffers for microphone
    //
    CreateQueue(&m_pBeginMicrophoneQueuedBuffers , XVOICE_TOTAL_REGIONS , (DWORD)XMEDIAPACKET_STATUS_PENDING);
    if( NULL ==  m_pBeginMicrophoneQueuedBuffers)
    {
        DebugPrint("CDelayVoiceUnit::Inserted()...Not enough memory for buffers!\n");
        hr = E_OUTOFMEMORY;
        return hr;
    }

    //
    // Create the buffers for headphone
    //
    CreateQueue(&m_pBeginHeadphoneQueuedBuffers , XVOICE_TOTAL_REGIONS , XMEDIAPACKET_STATUS_SUCCESS);
    if( NULL ==  m_pBeginHeadphoneQueuedBuffers)
    {
        DebugPrint("CDelayVoiceUnit::Inserted()...Not enough memory for buffers!\n");
        hr = E_OUTOFMEMORY;
        return hr;
    }

    //
    // Feed the microphone with buffers; we enqueue all the buffers to the microphone which is going to fill 
    // them with data gradually
    //
    MEDIA_PACKET_LIST *pTmpMBuff = m_pBeginMicrophoneQueuedBuffers;
    
    for( int i = 0 ; i < XVOICE_TOTAL_REGIONS ; i++ )
    {
        
        QueueMicrophoneBuffer(pTmpMBuff);
        pTmpMBuff = pTmpMBuff->pNext;
    }

    //
    // We don't enqueue buffers to the headphone yet; this is going to be done when buffers from the microphone
    // start to arrive
    //

    return S_OK;
}

void CDelayVoiceUnit::Removed()
{
    CDelayVoiceUnit::Cleanup();
}

void CDelayVoiceUnit::QueueMicrophoneBuffer(MEDIA_PACKET_LIST *pMediaPacketListEntry)
{
    XMEDIAPACKET *pMediaPacket = &(pMediaPacketListEntry->MediaPacket);

    //
    //  Setup the pointers to our status and completed size pointers
    //

    pMediaPacket->pdwCompletedSize = &(pMediaPacketListEntry->dwCompletedSize);
    pMediaPacket->pdwStatus = &(pMediaPacketListEntry->dwStatus);

    //
    //  Initialize the event 
    //

    pMediaPacket->hCompletionEvent = m_hMicrophoneWorkEvent;

    //
    //  Submit the buffer
    //
    VERIFY_SUCCESS(m_pMicrophone->Process(NULL, pMediaPacket));


}

void CDelayVoiceUnit::QueueHeadphoneBuffer(MEDIA_PACKET_LIST *pMediaPacketListEntry)
{
    XMEDIAPACKET *pMediaPacket = &(pMediaPacketListEntry->MediaPacket);

    //
    //  Setup the pointers to our status and completed size pointers
    //

    pMediaPacket->pdwCompletedSize = &(pMediaPacketListEntry->dwCompletedSize);
    pMediaPacket->pdwStatus = &(pMediaPacketListEntry->dwStatus);

    //
    //  Initialize the event
    //
    pMediaPacket->hCompletionEvent = m_hHeadphoneWorkEvent;

    //
    //  Submit the buffer
    //
    VERIFY_SUCCESS(m_pHeadphone->Process(pMediaPacket, NULL));

}

void CDelayVoiceUnit::SendPacketToRenderer()
{
    memcpy((m_pBeginHeadphoneQueuedBuffers->MediaPacket).pvBuffer ,
           (m_pBeginMicrophoneQueuedBuffers->MediaPacket).pvBuffer , 
            XVOICE_BUFFER_REGION_SIZE);
    m_pBeginHeadphoneQueuedBuffers->dwStatus = (DWORD)XMEDIAPACKET_STATUS_PENDING;
}

BOOL CDelayVoiceUnit::IsPacketPreparedForRender()
{
    return (XMEDIAPACKET_STATUS_PENDING == m_pBeginHeadphoneQueuedBuffers->dwStatus);
}

HRESULT CDelayVoiceUnit::OnMicrophoneReadComplete()
{
    HRESULT hr;
    
    //
    // One microphone buffer is filled with data
    //
    if(XMEDIAPACKET_STATUS_PENDING != m_pBeginMicrophoneQueuedBuffers->dwStatus)
    {
        if( SUCCEEDED( m_pBeginMicrophoneQueuedBuffers->dwStatus ) )
        {
            //
            // Send the packet to the headphone
            //
            SendPacketToRenderer();
            
        }

        if( XMEDIAPACKET_STATUS_FAILURE == m_pBeginMicrophoneQueuedBuffers->dwStatus )
        {
            hr = E_FAIL;
            return hr;
        }
        else
        {
            //
            // Requeue the packet
            //  
            m_pBeginMicrophoneQueuedBuffers->dwStatus = (DWORD)XMEDIAPACKET_STATUS_PENDING;
            QueueMicrophoneBuffer(m_pBeginMicrophoneQueuedBuffers);
            m_pBeginMicrophoneQueuedBuffers = m_pBeginMicrophoneQueuedBuffers->pNext;
        }

    }

    return S_OK;
}

HRESULT CDelayVoiceUnit::PlayToHeadphone()
{
    if(IsPacketPreparedForRender())
    {
        QueueHeadphoneBuffer(m_pBeginHeadphoneQueuedBuffers);
    }

    //
    // Requeue the packet
    //  
    m_pBeginHeadphoneQueuedBuffers = m_pBeginHeadphoneQueuedBuffers->pNext;

    return (S_OK);
}