#include "voice.h"

#define DSOUND_INCLUDE_XHAWK


//////////////////////////////////////////////////////////////////////////////
//
// Implementation of CVoiceUnit
//
//////////////////////////////////////////////////////////////////////////////

CVoiceUnit::CVoiceUnit():
        m_pMicrophone(NULL),
        m_pHeadphone(NULL),
        m_dwPortNumber(0),
        m_hMicrophoneWorkEvent(NULL),
        m_hHeadphoneWorkEvent(NULL),
        m_pBeginMicrophoneQueuedBuffers(NULL),
        m_pBeginHeadphoneQueuedBuffers(NULL),
/*#ifdef _DEBUG
        m_hFileRawPCMData(NULL) ,
        m_hFileCompressed(NULL) ,
        m_hFileDecompressed(NULL) , 
#endif*/
        m_fPreBuffering(FALSE),
        m_dwPreBufferRegions(0)
{}

void CVoiceUnit::Cleanup()
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

/*#ifdef _DEBUG
	CloseFile(m_hFileRawPCMData);
	CloseFile(m_hFileCompressed);
	CloseFile(m_hFileDecompressed);
#endif*/
}

void CVoiceUnit::DeleteQueue(MEDIA_PACKET_LIST *pQueue)
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

CVoiceUnit::~CVoiceUnit()
{
    Cleanup();
}

void CVoiceUnit::CreateQueue(MEDIA_PACKET_LIST **pQueue , WORD wSize , DWORD dwStatus)
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

void CVoiceUnit::Init(DWORD dwPortNumber)
{
	m_dwPortNumber = dwPortNumber;
}

void CVoiceUnit::Process()
{
    DWORD dwReason;
	HRESULT hr;

        if( NULL == m_hMicrophoneWorkEvent )
        {
            return; // No hawk inserted
        }

        do
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
        while(TRUE);
    
}

void CVoiceUnit::SetSockAddr(const IN_ADDR &addrPlayer)
{
}

HRESULT CVoiceUnit::Inserted()
{
    HRESULT         hr;
    WAVEFORMATEX    wfxFormat;
    
    do
    {
        DebugPrint("CVoiceUnit::Inserted()...Hawk Inserted\n");

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
        DebugPrint("CVoiceUnit::Inserted()...Creating microphone...\n");
        hr = XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_MICROPHONE, m_dwPortNumber , XVOICE_TOTAL_REGIONS,
                                     &wfxFormat, &m_pMicrophone);

        if( FAILED( hr ) )
        {
            DebugPrint("CVoiceUnit::Inserted()...Microphone failed with %x", hr);
            break;
        }

        DebugPrint("CVoiceUnit::Inserted()...Creating headphone...\n");
        hr = XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_HEADPHONE, m_dwPortNumber , XVOICE_TOTAL_REGIONS,
                                     &wfxFormat, &m_pHeadphone);

        if( FAILED( hr ) )
        {
            DebugPrint("CVoiceUnit::Inserted()...Headphone failed with %x", hr);
            m_pMicrophone->Release();
            m_pMicrophone = NULL;
            break;
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
            DebugPrint("CVoiceUnit::Inserted()...Not enough memory for buffers!\n");
            hr = E_OUTOFMEMORY;
            break;
        }

        //
        // Create the buffers for headphone
        //
        CreateQueue(&m_pBeginHeadphoneQueuedBuffers , XVOICE_TOTAL_REGIONS , XMEDIAPACKET_STATUS_SUCCESS);
        if( NULL ==  m_pBeginHeadphoneQueuedBuffers)
        {
            DebugPrint("CVoiceUnit::Inserted()...Not enough memory for buffers!\n");
            hr = E_OUTOFMEMORY;
            break;
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

        hr = S_OK;
    }
    while( FALSE );

    return( hr );
}

void CVoiceUnit::Removed()
{
    CVoiceUnit::Cleanup();
}

void CVoiceUnit::QueueMicrophoneBuffer(MEDIA_PACKET_LIST *pMediaPacketListEntry)
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

void CVoiceUnit::QueueHeadphoneBuffer(MEDIA_PACKET_LIST *pMediaPacketListEntry)
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

void CVoiceUnit::SendPacketToRenderer()
{
	memcpy((m_pBeginHeadphoneQueuedBuffers->MediaPacket).pvBuffer ,
		   (m_pBeginMicrophoneQueuedBuffers->MediaPacket).pvBuffer , 
            XVOICE_BUFFER_REGION_SIZE);
	m_pBeginHeadphoneQueuedBuffers->dwStatus = (DWORD)XMEDIAPACKET_STATUS_PENDING;
}

BOOL CVoiceUnit::IsPacketPreparedForRender()
{
	return (XMEDIAPACKET_STATUS_PENDING == m_pBeginHeadphoneQueuedBuffers->dwStatus);
}

HRESULT CVoiceUnit::OnMicrophoneReadComplete()
{
/*#ifdef _DEBUG
	static OVERLAPPED overlapped;
#endif*/
	
	HRESULT hr;
    
    do
    {
        //
        // One microphone buffer is filled with data
        //
	    if(XMEDIAPACKET_STATUS_PENDING != m_pBeginMicrophoneQueuedBuffers->dwStatus)
        {
            if( SUCCEEDED( m_pBeginMicrophoneQueuedBuffers->dwStatus ) )
            {
/*#ifdef _DEBUG
				WriteToLogFile(m_hFileRawPCMData , (BYTE*)(m_pBeginMicrophoneQueuedBuffers->MediaPacket).pvBuffer ,
				           XVOICE_BUFFER_REGION_SIZE , &overlapped);
#endif*/
                //
                // Send the packet to the headphone
                //
				SendPacketToRenderer();
                
            }

			if( XMEDIAPACKET_STATUS_FAILURE == m_pBeginMicrophoneQueuedBuffers->dwStatus )
            {
				hr = E_FAIL;
				break;
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
   
        hr = S_OK;
    }
    while( FALSE );
    
    return( hr );

}

HRESULT CVoiceUnit::PlayToHeadphone()
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

/*#ifdef _DEBUG

void CVoiceUnit::OpenFile(const char *szFileName, HANDLE *phFile)
{
	*phFile = CreateFile(szFileName,
					  GENERIC_WRITE,                // open for writing 
					  0,                            // do not share 
					  NULL,                         // no security 
					  CREATE_ALWAYS,                // overwrite existing 
					  FILE_ATTRIBUTE_NORMAL |       // normal file 
					  FILE_FLAG_OVERLAPPED,         // asynchronous I/O 
					  NULL);                        // no attr. template 

	if (*phFile == INVALID_HANDLE_VALUE) 
	{ 
		DWORD dwErr = GetLastError();
		DebugPrint("Error: %d.\n" , dwErr);  // process error 
	} 
}

void CVoiceUnit::CloseFile(HANDLE hFile)
{
	if( NULL != hFile )
    {
		// Make sure the message makes it to the disk
		FlushFileBuffers( hFile );
		CloseHandle(hFile); 
    }
}

void CVoiceUnit::WriteToLogFile(HANDLE hFile, BYTE *pbBuffer, DWORD dwSize , LPOVERLAPPED lpOverlapped)
{
static DWORD dwBytesWritten = 0;

	WriteFile( hFile,				      // handle to output file
			   pbBuffer,                      // data buffer
			   dwSize,                // number of bytes to write
			   &dwBytesWritten ,
			   lpOverlapped);

	lpOverlapped->Offset += dwBytesWritten;
}
#endif*/

//////////////////////////////////////////////////////////////////////////////
//
// Implementation of CCompressedVoiceUnit
//
//////////////////////////////////////////////////////////////////////////////
CCompressedVoiceUnit::CCompressedVoiceUnit() : 
                        CVoiceUnit() , 
                        m_pEncoder(NULL) , 
                        m_pDecoder(NULL) ,
                        m_hEncodeWorkEvent(NULL),
                        m_hDecodeWorkEvent(NULL),
					    m_bInPlaceCodec(FALSE)
{
	for( int i = 0 ; i < VOICE_DEVICES_COUNT ; i++ )
    {
		m_CodecMediaBuffer[i].pvBuffer = NULL;
    }
}

void CCompressedVoiceUnit::Cleanup()
{
    if(m_pEncoder)
    {
        m_pEncoder->Release();
        m_pEncoder = NULL;
    }
    if(m_pDecoder)
    {
        m_pDecoder->Release();
        m_pDecoder = NULL;
    }

	for( int i = 0 ; i < VOICE_DEVICES_COUNT ; i++ )
    {
		if( NULL != m_CodecMediaBuffer[i].pvBuffer )
		{
			delete [] m_CodecMediaBuffer[i].pvBuffer;
			m_CodecMediaBuffer[i].pvBuffer = NULL;
		}
	}
    if (m_hEncodeWorkEvent) 
    {
        CloseHandle(m_hEncodeWorkEvent);
        m_hEncodeWorkEvent = NULL;
    }

    if (m_hDecodeWorkEvent) 
    {
        CloseHandle(m_hDecodeWorkEvent);
        m_hDecodeWorkEvent = NULL;
    }

}

CCompressedVoiceUnit::~CCompressedVoiceUnit()
{
    Cleanup();
}

void CCompressedVoiceUnit::Removed()
{
    CCompressedVoiceUnit::Cleanup();

    CVoiceUnit::Removed();
}

HRESULT CCompressedVoiceUnit::Inserted()
{
    HRESULT hr;
    XMEDIAINFO      xMediaInfo;

    WAVEFORMATEX wfxFormat;
    wfxFormat.wFormatTag = WAVE_FORMAT_PCM;
    wfxFormat.nChannels = 1;
    wfxFormat.nSamplesPerSec = VOICE_SAMPLING_RATE;
    wfxFormat.nAvgBytesPerSec = 2 * VOICE_SAMPLING_RATE;
    wfxFormat.nBlockAlign = 2;
    wfxFormat.wBitsPerSample = 16;
    wfxFormat.cbSize = 0;
    
    do
    {
        hr = CVoiceUnit::Inserted();

        if( FAILED( hr ) )
        {
            DebugPrint("CCompressedVoiceUnit::Inserted()...Base class Inserted() failed!\n");
            break;
        }

        DebugPrint("Creating voice encoder...\n");

        hr = XVoiceEncoderCreateMediaObject( FALSE, // manual mode
                                             WAVE_FORMAT_VOXWARE_SC06,
                                             &wfxFormat,
                                             20, // voice activation threshold
                                             &m_pEncoder );

        if( FAILED( hr ) )
        {
            DebugPrint("CCompressedVoiceUnit::Inserted()...Voice encoder failed with %x", hr);
            break;
        }

        DebugPrint(("Creating voice decoder...\n"));

        hr = XVoiceDecoderCreateMediaObject( 0, // zero latency
                                             WAVE_FORMAT_VOXWARE_SC06,
                                             &wfxFormat,
                                             &m_pDecoder );
        if( FAILED( hr ) )
        {
            DebugPrint("CCompressedVoiceUnit::Inserted()...Voice decoder failed with %x", hr);
            m_pEncoder->Release();
            m_pEncoder = NULL;
            break;
        }
    
                    
        //
        // If the codec XMOs are in place don't pass a DST temp buffer...
        //

        m_pEncoder->GetInfo( &xMediaInfo );

        if (xMediaInfo.dwFlags & XMO_STREAMF_IN_PLACE) 
        {
            DebugPrint("CCompressedVoiceUnit::Inserted()...Encoder %x is in place\n", m_pEncoder);
			m_bInPlaceCodec = TRUE;

        } 
        else 
        {
			for( int i = 0 ; i < VOICE_DEVICES_COUNT ; i++ )
			{
				memset(&(m_CodecMediaBuffer[i]), 0, sizeof(XMEDIAPACKET));

				m_CodecMediaBuffer[i].dwMaxSize = XVOICE_CODEC_BUFFER_SIZE;
				m_CodecMediaBuffer[i].pvBuffer = new BYTE[XVOICE_CODEC_BUFFER_SIZE];
				assert(m_CodecMediaBuffer[i].pvBuffer);

			}
            DebugPrint("CCompressedVoiceUnit::Inserted()...Codec xmos are not in place, using temp buffer\n");
        }

        m_hEncodeWorkEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        m_hDecodeWorkEvent = CreateEvent(NULL, FALSE, FALSE, NULL);


        hr = S_OK;
    }
    while( FALSE );
    
    return( hr );
}

HRESULT CCompressedVoiceUnit::OnMicrophoneReadComplete()
{
	BOOL bVoiceDetected = TRUE;

/*#ifdef _DEBUG
	static OVERLAPPED overlapped;
#endif*/


	HRESULT hr;
    
    do
    {
		//
		// One microphone buffer is filled with data
		//
		if(XMEDIAPACKET_STATUS_PENDING != m_pBeginMicrophoneQueuedBuffers->dwStatus)
		{
			if( SUCCEEDED( m_pBeginMicrophoneQueuedBuffers->dwStatus ) )
			{
/*#ifdef _DEBUG
				WriteToLogFile(m_hFileRawPCMData , (BYTE*)(m_pBeginMicrophoneQueuedBuffers->MediaPacket).pvBuffer ,
							   XVOICE_BUFFER_REGION_SIZE , &overlapped);
#endif*/

				//
				// 1. Compress media buffer
				//
				CompressMediaBuffer(&m_pBeginMicrophoneQueuedBuffers->MediaPacket ,
					                &m_CodecMediaBuffer[MICROPHONE] , &bVoiceDetected);
				memset((m_pBeginMicrophoneQueuedBuffers->MediaPacket).pvBuffer , 0 , XVOICE_BUFFER_REGION_SIZE); 


				if(bVoiceDetected)
				{
					//
					// 2. Prepare the packet for the output device (headphone)
					//
					SendPacketToRenderer();
				}
											
			}

			if( XMEDIAPACKET_STATUS_FAILURE == m_pBeginMicrophoneQueuedBuffers->dwStatus )
            {
				hr = E_FAIL;
				break;
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
    
        hr = S_OK;
    }
    while( FALSE );
    
    return( hr );
}

HRESULT CCompressedVoiceUnit::PlayToHeadphone()
{

	if(IsPacketPreparedForRender())    
	{
		//
		// 1. Decompress media buffer
		//
		DecompressMediaBuffer(&m_CodecMediaBuffer[HEADPHONE] , &m_pBeginHeadphoneQueuedBuffers->MediaPacket);

		//
		// 2. Send it to the headphone
		//
       	QueueHeadphoneBuffer(m_pBeginHeadphoneQueuedBuffers);
	}

    //
    // Requeue the packet
    //  
    m_pBeginHeadphoneQueuedBuffers = m_pBeginHeadphoneQueuedBuffers->pNext;

	return (S_OK);
}

HRESULT CCompressedVoiceUnit::CompressMediaBuffer(XMEDIAPACKET *pSrc, XMEDIAPACKET *pDst , BOOL *pvbVoiceDetected)
{
    HRESULT hr;
    DWORD dwSizeSrc = 0; 
    DWORD dwSizeDst = 0; 

/*#ifdef _DEBUG
	static OVERLAPPED overlapped;
#endif*/

    memset(pDst->pvBuffer, 0 , XVOICE_CODEC_BUFFER_SIZE);

    pDst->pdwCompletedSize = &dwSizeDst;
    pDst->hCompletionEvent = m_hEncodeWorkEvent;

    if( m_bInPlaceCodec )
    {
		//
		// Call encoding xmo
		//
		memcpy(pDst->pvBuffer , pSrc->pvBuffer , pSrc->dwMaxSize);
		hr = m_pEncoder->Process(NULL , pDst);

    }
	else
	{
	    //
	    // Call encoding xmo
		//
		pSrc->pdwCompletedSize = &dwSizeSrc;
		hr = m_pEncoder->Process( pSrc , pDst);
	}

    assert(!FAILED(hr));
    
    WaitForSingleObject(m_hEncodeWorkEvent , INFINITE);

/*#ifdef _DEBUG
	WriteToLogFile(m_hFileCompressed , (BYTE*)pDst->pvBuffer , dwSizeDst , &overlapped);
#endif*/

    if( dwSizeDst == sizeof(VOICE_ENCODER_HEADER) )
    {
		*pvbVoiceDetected = FALSE;
    }
	else
	{
		*pvbVoiceDetected = TRUE;
	}
    return( hr );
}

HRESULT CCompressedVoiceUnit::DecompressMediaBuffer(XMEDIAPACKET *pSrc, XMEDIAPACKET *pDst)
{
    HRESULT hr;
    DWORD dwSizeSrc = 0; 
    DWORD dwSizeDst = 0; 

/*#ifdef _DEBUG
	static OVERLAPPED overlapped;
#endif*/
    
    memset(pDst->pvBuffer , 0 , pDst->dwMaxSize);


    pDst->pdwCompletedSize = &dwSizeDst;
    pDst->hCompletionEvent = m_hDecodeWorkEvent;

    if( m_bInPlaceCodec )
    {
		//
		// Call decoding xmo
		//
		memcpy(pDst->pvBuffer , pSrc->pvBuffer , pSrc->dwMaxSize);
		hr = m_pDecoder->Process(NULL , pDst);

    }
	else
	{
	    //
	    // Call decoding xmo
		//
		pSrc->pdwCompletedSize = &dwSizeSrc;
		hr = m_pDecoder->Process( pSrc , pDst);
	}
    
    assert(!FAILED(hr));

    WaitForSingleObject(m_hDecodeWorkEvent , INFINITE);

/*#ifdef _DEBUG
	WriteToLogFile(m_hFileDecompressed , (BYTE*)pDst->pvBuffer , dwSizeDst , &overlapped);
#endif*/

    return( hr );
}

void CCompressedVoiceUnit::SendPacketToRenderer()
{
	m_pBeginHeadphoneQueuedBuffers->dwStatus = (DWORD)XMEDIAPACKET_STATUS_PENDING;
	memcpy(m_CodecMediaBuffer[HEADPHONE].pvBuffer , m_CodecMediaBuffer[MICROPHONE].pvBuffer , XVOICE_CODEC_BUFFER_SIZE);
	memset(m_CodecMediaBuffer[MICROPHONE].pvBuffer , 0 , XVOICE_CODEC_BUFFER_SIZE);
}

BOOL CCompressedVoiceUnit::IsPacketPreparedForRender()
{
	return(XMEDIAPACKET_STATUS_PENDING == m_pBeginHeadphoneQueuedBuffers->dwStatus);
}

//////////////////////////////////////////////////////////////////////////////
//
// Implementation of CNetVoiceUnit
//
//////////////////////////////////////////////////////////////////////////////
CNetVoiceUnit::CNetVoiceUnit() : CCompressedVoiceUnit() ,
                                 m_VoiceSock(INVALID_SOCKET),
								 m_pVoiceQueuingXMO(NULL)
{
    m_SockAddrDest.sin_family = AF_INET;
    m_SockAddrDest.sin_addr.s_addr = htonl(INADDR_ANY);
    m_SockAddrDest.sin_port = htons( VOICE_PORT );
}

void CNetVoiceUnit::Cleanup()
{
    if( INVALID_SOCKET != m_VoiceSock )
    {
        closesocket( m_VoiceSock );
        m_VoiceSock = INVALID_SOCKET;
    }

	if( NULL != m_pVoiceQueuingXMO )
    {
		m_pVoiceQueuingXMO->Release();
		m_pVoiceQueuingXMO = NULL;
    }
}

CNetVoiceUnit::~CNetVoiceUnit()
{
    Cleanup();
}

void CNetVoiceUnit::SetSockAddr(const IN_ADDR &addrPlayer)
{
	m_SockAddrDest.sin_family = AF_INET;
    m_SockAddrDest.sin_addr = addrPlayer;
    m_SockAddrDest.sin_port = htons( VOICE_PORT );
	
}

void CNetVoiceUnit::Removed()
{
    CNetVoiceUnit::Cleanup();

    CCompressedVoiceUnit::Removed();
}


HRESULT CNetVoiceUnit::Inserted()
{
    HRESULT hr;
    
    do
    {
		//
        // Base class initialization
        //
        hr = CCompressedVoiceUnit::Inserted();

        if( FAILED( hr ) )
        {
            DebugPrint("CNetVoiceUnit::Inserted()...Base class Inserted() failed!\n");
            break;
        }

		//
        // Network stuff initialization
        //
        m_VoiceSock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
        if( INVALID_SOCKET ==  m_VoiceSock )
        {
            hr = WSAGetLastError();
            DebugPrint("CNetVoiceUnit::Inserted()...Voice socket open; error = %x\n", hr);
            break;
        }

        SOCKADDR_IN voiceAddr;
		voiceAddr.sin_family = AF_INET;
		voiceAddr.sin_addr.s_addr = /*htonl(*/INADDR_ANY;
		voiceAddr.sin_port = htons( VOICE_PORT );
        int iResult = bind( m_VoiceSock, (const sockaddr*)(&voiceAddr), sizeof( SOCKADDR_IN ) );
        if(iResult == SOCKET_ERROR)
        {
           hr = WSAGetLastError();
           DebugPrint( "CNetVoiceUnit::Inserted()...Voice socket open; error = %x\n", hr);
           break;
        }

		DWORD dwNonBlocking = 1;
		iResult = ioctlsocket(m_VoiceSock , FIONBIO, &dwNonBlocking );
		if(iResult == SOCKET_ERROR)
		{
           hr = WSAGetLastError();
           DebugPrint( "CNetVoiceUnit::Inserted()...Voice socket open; error = %x\n", hr);
           break;
		}

		//
        // Voice queueing initialization
        //
		QueueXMOConfig config;
		config.cbSize = sizeof(config);
		config.dIdealQuality = 0.5;
		config.dInitialFrameStrength = 0.05;

		hr = XQueueCreateMediaObject(&config , &m_pVoiceQueuingXMO);
		
		if( FAILED( hr ) )
        {
			DebugPrint("CNetVoiceUnit::Inserted()...Voice queuing initialization failed; error = %x\n", hr);
			break;
        }

		//
        // Output is requested from the queuing XMO only if m_IntervalForOutput time has elapsed
		//
		m_IntervalForOutput = config.wMsOfDataPerPacket / 2;
		m_LastOutputTimeStamp = GetTickCount();

        hr = S_OK;
    }
    while( FALSE );
    
    return( hr );
}


HRESULT CNetVoiceUnit::PlayToHeadphone()
{
	HRESULT hr = E_FAIL; // S_OK = Means decompression OK


	if(IsPacketPreparedForRender())    
	{
		//
		// 1. Call the queuing system with the codec buffer as input
		//
		hr = m_pVoiceQueuingXMO->Process(&m_CodecMediaBuffer[HEADPHONE] , &m_pBeginHeadphoneQueuedBuffers->MediaPacket);

	}
	else
	{
		//
        // 1. Call for output only if the interval elapsed
        //
		if(GetTickCount() - m_LastOutputTimeStamp >= m_IntervalForOutput)
		{
			hr = m_pVoiceQueuingXMO->Process(NULL , &m_pBeginHeadphoneQueuedBuffers->MediaPacket);
		}
	}
	m_LastOutputTimeStamp = GetTickCount();

	if( SUCCEEDED( hr ) )
    {
		//
		// 2. Send it to the headphone
		//
  		QueueHeadphoneBuffer(m_pBeginHeadphoneQueuedBuffers);
    }


    //
    // Requeue the packet
    //  
    m_pBeginHeadphoneQueuedBuffers = m_pBeginHeadphoneQueuedBuffers->pNext;

	return (S_OK);
}


void CNetVoiceUnit::SendPacketToRenderer()
{
	SendVoicePacket( &m_CodecMediaBuffer[MICROPHONE] );
}

BOOL CNetVoiceUnit::IsPacketPreparedForRender()
{
	return RecvVoicePacket(&m_CodecMediaBuffer[HEADPHONE]);
}

void CNetVoiceUnit::SendVoicePacket( XMEDIAPACKET *pMediaPacket )
{

	int nBytes = sendto( m_VoiceSock , (const char*)pMediaPacket->pvBuffer, XVOICE_CODEC_BUFFER_SIZE , 
                         0, (const sockaddr*)(&m_SockAddrDest), sizeof( SOCKADDR_IN ) );

	DebugPrint("Sent: %d bytes\n" , nBytes);

    assert( nBytes == XVOICE_CODEC_BUFFER_SIZE );
}

BOOL CNetVoiceUnit::RecvVoicePacket( XMEDIAPACKET *pMediaPacket )
{

    // See if a voice message is waiting for us
    SOCKADDR_IN saFromIn;
	int iSize = sizeof( SOCKADDR_IN );
	memset(pMediaPacket->pvBuffer , 0 , XVOICE_CODEC_BUFFER_SIZE);
    int iResult = recvfrom( m_VoiceSock , (char*)pMediaPacket->pvBuffer, XVOICE_CODEC_BUFFER_SIZE, 
							0, (sockaddr*)(&saFromIn), &iSize );
    

    // If message waiting, process it
    if( iResult != SOCKET_ERROR && iResult > 0 )
    {
        DebugPrint("Recv: %d bytes\n" , iResult);
        return TRUE;
    }

	DebugPrint("Recv Error: %d \n" , WSAGetLastError());
    return FALSE;
}