//-----------------------------------------------------------------------------
// File: XVoiceChat.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "XVoiceChatp.h"

//////////////////////////////////////////////////////////////////////////////
/*XOnlineVChatStartup - Routine Description:

    Initializes the voice chat manager on the xbox. 

Arguments:
    pConfig[in]            Pointer to the configuration structure.
    phVChatMgr[out]        Handle to the created voice chat manager.

Return Value:

    HRESULT

Memory Allocation:

    Allocates voice chat manager.
*/
//////////////////////////////////////////////////////////////////////////////
XBOXAPI
HRESULT
WINAPI 
XOnlineVChatStartup(
       IN  LPVOICE_CHAT_CONFIG   pConfig, 
	   OUT PHANDLE               phVChatMgr
	   )
{
    HRESULT hr = S_OK;

    assert(NULL != pConfig);
    assert(NULL != phVChatMgr);
    
    XONVCHAT_MGR_HANDLE hVChatMgr = NULL;

    *phVChatMgr = NULL;    

   
    do
    {
        //
        // Allocate space for the voice chat manager structures in memory
        //
        hVChatMgr = new CCommunicatorMgr;
        if( NULL ==  hVChatMgr)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        //
        // Initialize the voice chat manager structure
        //
        hr = hVChatMgr->Initialize(pConfig);
        if( FAILED( hr ) )
        {
            break;
        }
    }
    while( FALSE );
    
    if( FAILED( hr ) )
    {
        if( NULL != hVChatMgr )
        {
            //
            // Cleanup allocated memory
            //
            delete hVChatMgr;
        }
    }
    else
    {
        //
        //  Return the handle to the newly created voice chat manager
        //
        *phVChatMgr = hVChatMgr;
    }

    return( hr );
}

//////////////////////////////////////////////////////////////////////////////
/*XOnlineVChatChatterAdd - Routine Description:

    Adds a player from a team to the voice chat going on on this machine. 

Arguments:
    hVChatMgr[in]            Handle to the voice chat manager.
    bTeamNo[in]              Team # in which the player will be added.
	bPlayerNo[in]            Player # whithin a game ( 0 - 256 ; maximum players in a game 16 )
	bHeadsetPort[in]         Headset port (0 - 3) if the player is on this machine, 0xFF if it's not

Return Value:

    HRESULT

Memory Allocation:

    Allocates CCommunicator resources for a player on the local machine.
	Allocates a QueueXMO structure for a player on another XBOX.
*/
//////////////////////////////////////////////////////////////////////////////
XBOXAPI
HRESULT
WINAPI 
XOnlineVChatChatterAdd(
       IN HANDLE                   hVChatMgr, 
	   IN BYTE                     bTeamNo, 
	   IN BYTE                     bPlayerNo, 
	   IN BYTE                     bHeadsetPort
	   )
{
	return (S_OK);
}

//////////////////////////////////////////////////////////////////////////////
/*XOnlineVChatChatterRemove - Routine Description:

    Removes a player from the voice chat going on on this machine. 

Arguments:
    hVChatMgr[in]            Handle to the voice chat manager.
	bPlayerNo[in]            Player # whithin a game ( 0 - 256 ; maximum players in a game 16 )
	
Return Value:

    HRESULT

Memory Allocation:

    No. (Frees the memory allocated to the player's resources)
*/
//////////////////////////////////////////////////////////////////////////////
XBOXAPI
HRESULT
WINAPI 
XOnlineVChatChatterRemove(
       IN HANDLE                   hVChatMgr, 
	   IN BYTE                     bPlayerNo
	   )
{
	return (S_OK);
}

//////////////////////////////////////////////////////////////////////////////
/*XOnlineVChatChatterGetStates - Routine Description:

    Tells the client which are the current state of the communicators on each port
	(connected/not connected).

Arguments:
    hVChatMgr[in]            Handle to the voice chat manager.
	pbStatus[out]            The last four bytes represent the status of the communicators
	                         To extract specific information, the client should apply the 
							 masks defined by COMMUNICATORS array.
Return Value:

    HRESULT

Memory Allocation:

    No. 
*/
//////////////////////////////////////////////////////////////////////////////
XBOXAPI
HRESULT
WINAPI 
XOnlineVChatChatterGetStates(
       IN HANDLE                   hVChatMgr, 
	   OUT PBYTE                   pbStatus
	   )
{
	return (S_OK);
}

//////////////////////////////////////////////////////////////////////////////
/*XOnlineVChatMute - Routine Description:

    Mutes a certain chatter for one of the players.

Arguments:
    hVChatMgr[in]            Handle to the voice chat manager.
	bMutedPlayerNo[in]       The player # to be muted.
    bMutingPlayerNo[in]      The player # that will see the effect of the muting.

Return Value:

    HRESULT

Memory Allocation:

    No. 
*/
//////////////////////////////////////////////////////////////////////////////
XBOXAPI
HRESULT
WINAPI 
XOnlineVChatMute(
       IN HANDLE                   hVChatMgr, 
	   IN BYTE                     bMutedPlayerNo, 
	   IN BYTE                     bMutingPlayerNo
	   )
{
	return (S_OK);
}

//////////////////////////////////////////////////////////////////////////////
/*XOnlineVChatVoiceEffect - Routine Description:

    Applies a voice effect to one player's voice.

Arguments:
    hVChatMgr[in]            Handle to the voice chat manager.
	bPlayerNo[in]            The player # whose voice will be masked.
    voiceEffect[in]          The voice effect applied (from the VOICE_EFFECTS 
	                         enumeration).

Return Value:

    HRESULT

Memory Allocation:

    No. 
*/
//////////////////////////////////////////////////////////////////////////////
XBOXAPI
HRESULT
WINAPI 
XOnlineVChatVoiceEffect(
       IN HANDLE                   hVChatMgr, 
	   IN BYTE                     bPlayerNo, 
	   IN VOICE_EFFECTS            voiceEffect
	   )
{
	return (S_OK);
}

//////////////////////////////////////////////////////////////////////////////
/*XOnlineVChatPacketReceive - Routine Description:

    Gives the voice chat manager the packet that came from the network. The mgr
	breaks the big network packet into pieces and distributes them into the appropriate
	queues. No voice processing involved.

Arguments:
    hVChatMgr[in]            Handle to the voice chat manager.
	pvNetworkPacketIn[in]    Pointer to the network packet.
    bNetworkPacketInSize[in] Size of the network packet.

Return Value:

    HRESULT

Memory Allocation:

    No. 
*/
//////////////////////////////////////////////////////////////////////////////
XBOXAPI
HRESULT
WINAPI 
XOnlineVChatPacketReceive(
       IN HANDLE                   hVChatMgr, 
	   IN PVOID                    pvNetworkPacketIn, 
	   IN BYTE                     bNetworkPacketInSize
	   )
{
	return (S_OK);
}

//////////////////////////////////////////////////////////////////////////////
/*XOnlineVChatPacketSend - Routine Description:

    Copies the current network packet internally created to a buffer provided 
	by the client.

Arguments:
    hVChatMgr[in]             Handle to the voice chat manager.
	pvNetworkPacketOut[in]    Pointer to the network packet.
    bNetworkPacketOutSize[in] Size of the network packet.

Return Value:

    HRESULT

Memory Allocation:

    No. 
*/
//////////////////////////////////////////////////////////////////////////////
XBOXAPI
HRESULT
WINAPI 
XOnlineVChatPacketSend(
       IN HANDLE                   hVChatMgr, 
	   IN OUT PVOID                pvNetworkPacketOut, 
	   IN BYTE                     bNetworkPacketOutSize
	   )
{
	return (S_OK);
}

//////////////////////////////////////////////////////////////////////////////
/*XOnlineVChatProcess - Routine Description:

    Engine of the voice chat. Should be called once in the game infinite loop. 

Arguments:
    hVChatMgr[in]             Handle to the voice chat manager.

Return Value:

    HRESULT

Memory Allocation:

    No. 
*/
//////////////////////////////////////////////////////////////////////////////
XBOXAPI
HRESULT
WINAPI 
XOnlineVChatProcess(
       IN HANDLE                   hVChatMgr
	  )
{
	return (S_OK);
}

//////////////////////////////////////////////////////////////////////////////
/*XOnlineVChatCleanup - Routine Description:

    Cleans up the resources associated with a communicator manager and disables 
	voice chat on the local machine. 

Arguments:
    hVChatMgr[in]             Handle to the voice chat manager.

Return Value:

    HRESULT

Memory Allocation:

    No. (Frees memory associated with the resources of the voice chat manager).
*/
//////////////////////////////////////////////////////////////////////////////
XBOXAPI
HRESULT
WINAPI 
XOnlineVChatCleanup(
       IN HANDLE                   hVChatMgr
	   )
{
	assert(NULL != hVChatMgr);

	delete ((XONVCHAT_MGR_HANDLE)hVChatMgr);

	return (S_OK);
}

//////////////////////////////////////////////////////////////////////////////
//
// Implementation of CHeadsetXMediaPacket 
//
//////////////////////////////////////////////////////////////////////////////
VOID CHeadsetXMediaPacket::Initialize(PBYTE pbBuffer , DWORD dwSize , DWORD dwStatus, HANDLE hWorkEvent) 
{
	m_XMediaPacket.dwMaxSize = dwSize;

	assert(NULL != pbBuffer);

    m_XMediaPacket.pvBuffer = pbBuffer;
    memset(m_XMediaPacket.pvBuffer , 0 , dwSize);

	//
    // Initialize all other members
    //
	m_dwStatus = dwStatus;
	m_dwCompletedSize = 0;
}

VOID CHeadsetXMediaPacket::Dump()
{
	DebugPrint( "CHeadsetXMediaPacket - Dump object 0x%x :m_XMediaPacket.pvBuffer = 0x%x m_dwStatus = %d m_dwCompletedSize = %d\n",
		               this, m_XMediaPacket.pvBuffer, m_dwStatus, m_dwCompletedSize );
}

//////////////////////////////////////////////////////////////////////////////
//
// Implementation of CHeadsetXMediaPacketQueue 
//
//////////////////////////////////////////////////////////////////////////////
HRESULT CHeadsetXMediaPacketQueue::CreateHeadsetQueue(DWORD dwXMPCount, DWORD dwSize, DWORD dwStatus, HANDLE hWorkEvent)
{
	HRESULT hr = S_OK;
     
	assert( 0 != dwXMPCount);
    assert( 0 < dwSize && 640 >= dwSize);

    do
    {
		m_dwSize = dwSize;
		m_dwXMPCount = dwXMPCount;

		m_pbBufferSpace = new BYTE[ m_dwXMPCount * m_dwSize];

		if( NULL == m_pbBufferSpace )
		{
			hr = E_OUTOFMEMORY;
			break;
		}    

		m_pXMPQueue = new CHeadsetXMediaPacket[m_dwXMPCount];

		if( NULL == m_pXMPQueue )
        {
			hr = E_OUTOFMEMORY;
			break;
        }

		for( DWORD i = 0; i < m_dwXMPCount ; i++)
        {
			m_pXMPQueue[i].Initialize( (m_pbBufferSpace + i * m_dwSize) ,m_dwSize, dwStatus, hWorkEvent);
        }
    }
    while( FALSE );
       
    return( hr );
}

LPXMEDIAPACKET CHeadsetXMediaPacketQueue::GetCurrentXMP()
{
	assert(m_dwCurrentIndex < m_dwXMPCount);

	LPXMEDIAPACKET pXMP = m_pXMPQueue[m_dwCurrentIndex].GetXMP();

	m_dwCurrentIndex = (m_dwCurrentIndex + 1) % m_dwXMPCount;

	return (pXMP);
}

VOID CHeadsetXMediaPacketQueue::Cleanup()
{
	if( NULL != m_pbBufferSpace )
    {
		delete [] m_pbBufferSpace;
		m_pbBufferSpace = NULL;
		DebugPrint("Deleted headset buffer space\n" );
    }

	if( NULL != m_pXMPQueue )
    {
		delete [] m_pXMPQueue;
		m_pXMPQueue = NULL;
		DebugPrint( "Deleted headset queue\n" );
    }

	m_dwXMPCount = 0;
	m_dwSize = 0;
	m_dwCurrentIndex = 0;

	DebugPrint( "Headset queue cleanup done!\n");
}

VOID CHeadsetXMediaPacketQueue::Dump()
{
	DebugPrint( "CHeadsetXMediaPacketQueue - Dump object 0x%x: m_dwXMPCount = %d m_dwSize = %d m_pXMPQueue = 0x%x m_pbBufferSpace = %x m_dwCurrentIndex = %d\n",
		              this, m_dwXMPCount, m_dwSize, m_pXMPQueue, m_pbBufferSpace, m_dwCurrentIndex );

	for( DWORD i = 0; i < m_dwXMPCount ; i++ )
    {
		m_pXMPQueue[i].Dump();
    }
}

VOID CHeadsetXMediaPacketQueue::Verify()
{
	if( (NULL != m_pbBufferSpace) && ((0 == m_dwXMPCount) || (0 == m_dwSize)) )
    {
		DebugPrint("Headset queue error!\n");
		assert(FALSE);
    }

	if( (NULL == m_pbBufferSpace) && (0 != m_dwXMPCount) )
    {
		DebugPrint("Headset queue error!\n");
		assert(FALSE);
    }

	DebugPrint( "Verify...OK!\n");

}

//////////////////////////////////////////////////////////////////////////////
//
// Implementation of CCommunicator 
//
//////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Name: CCommunicator
// Desc: Constructor
//-----------------------------------------------------------------------------
CCommunicator::CCommunicator( ):
		m_bHeadsetPort(0),
        m_pMicrophone(NULL),
        m_pHeadphone(NULL),
        m_pEncoder(NULL),
		m_bChannel(0),
		m_bPlayerNo(0),
        m_bCommActive(FALSE)
        {           
        }

//-----------------------------------------------------------------------------
// Name: ~CCommunicator
// Desc: Destructor
//-----------------------------------------------------------------------------
CCommunicator::~CCommunicator()
{
    Cleanup();
}

//-----------------------------------------------------------------------------
// Name: Cleanup
// Desc: Called at destruction time or when the communicator is removed
//-----------------------------------------------------------------------------
VOID CCommunicator::Cleanup()
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

	DebugPrint( "Communicator 0x%x's cleanup done!\n", this);
}

//-----------------------------------------------------------------------------
// Name: Inserted
// Desc: Called when a communicator is newly detected in this slot, does all initializations
//-----------------------------------------------------------------------------
HRESULT CCommunicator::Inserted(
			LPCVChatParameters   pVChatParams,
			BYTE                 bHeadsetPort,
			HANDLE               hEvtMicInput
			)
{
    HRESULT         hr = S_OK;
    WAVEFORMATEX    wfxFormat;

	assert( (INVALID_HANDLE_VALUE != hEvtMicInput) && (NULL != hEvtMicInput));
	assert( NULL != pVChatParams );
	assert( (bHeadsetPort >= 0) && (bHeadsetPort < 4));

	m_bHeadsetPort = bHeadsetPort;

	//
    // Computes PCM buffer size from the current settings
    //
    WORD wVoiceBufferSize = pVChatParams->GetPCMBufferTime() * (((WORD)( pVChatParams->GetVoiceSamplingRate() / 1000.0 )) * 2 );
    
    DebugPrint( "CCommunicator::Inserted()...Xbox Communicator Inserted\n");

	do
	{

		//
		// The headset initialization
		//
		wfxFormat.wFormatTag = WAVE_FORMAT_PCM;
		wfxFormat.nChannels = 1;
		wfxFormat.nSamplesPerSec = pVChatParams->GetVoiceSamplingRate();
		wfxFormat.nAvgBytesPerSec = 2 * pVChatParams->GetVoiceSamplingRate();
		wfxFormat.nBlockAlign = 2;
		wfxFormat.wBitsPerSample = 16;
		wfxFormat.cbSize = 0;

		//
		//  Create a microphone and a headphone
		//
		DebugPrint( "CCommunicator::Inserted()...Creating microphone...\n");
		hr = XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_MICROPHONE, (DWORD)m_bHeadsetPort , PCM_BUFFERS_COUNT,
									 &wfxFormat, &m_pMicrophone);

		if( FAILED( hr ) )
		{
			DebugPrint( "CCommunicator::Inserted()...Microphone failed with %x", hr);
			break;
		}

		DebugPrint( "CCommunicator::Inserted()...Creating headphone...\n");
		hr = XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_HEADPHONE, (DWORD)m_bHeadsetPort , PCM_BUFFERS_COUNT,
									 &wfxFormat, &m_pHeadphone);

		if( FAILED( hr ) )
		{
			DebugPrint( "CCommunicator::Inserted()...Headphone failed with %x", hr);
			break;
		}

		DebugPrint( "CCommunicator::Inserted()...Creating voice encoder...\n");
		hr = XVoiceEncoderCreateMediaObject( FALSE, // manual mode
											 pVChatParams->GetCodecTag(),
											 &wfxFormat,
											 20, // voice activation threshold (0 - 255)
											 &m_pEncoder );

		if( FAILED( hr ) )
		{
			DebugPrint( "CCommunicator::Inserted()...Voice encoder failed with %x", hr);
			break;
		}

		//
		// Create the buffers for microphone
		//
		hr = m_MicrophoneQueue.CreateHeadsetQueue(PCM_BUFFERS_COUNT , 
			                                      wVoiceBufferSize, 
												  (DWORD)XMEDIAPACKET_STATUS_PENDING, 
												  hEvtMicInput);

		if( FAILED( hr ) )
		{
			DebugPrint( "CCommunicator::Inserted()...Create microphone queue failed with %x", hr);
			break;
		}


		//
		// Create the buffers for headphone
		//
		hr = m_HeadphoneQueue.CreateHeadsetQueue(PCM_BUFFERS_COUNT , 
			                                     wVoiceBufferSize, 
												 (DWORD)XMEDIAPACKET_STATUS_SUCCESS, 
												 NULL);

		if( FAILED( hr ) )
		{
			DebugPrint( "CCommunicator::Inserted()...Create headphone queue failed with %x", hr);
			break;
		}

		//
		// Feed the microphone with buffers; we enqueue all the buffers to the microphone which is going to fill 
		// them with data gradually
		//
		for( DWORD i = 0 ; i < PCM_BUFFERS_COUNT ; i++ )
		{
			LPXMEDIAPACKET pXMP = m_MicrophoneQueue.GetCurrentXMP();

			//
			//  Submit the buffer
			//
			hr = m_pMicrophone->Process(NULL, pXMP);

			assert(SUCCEEDED(hr));
		}

		//
		// We don't enqueue buffers to the headphone yet; this is going to be done when buffers from the microphone
		// start to arrive
		//

		//
        // Set the communicator active status to TRUE
        //
		m_bCommActive = TRUE;
	}
	while(FALSE);

    return (hr);
}

//-----------------------------------------------------------------------------
// Name: Removed
// Desc: Called when the communicator is unplugged
//-----------------------------------------------------------------------------
VOID CCommunicator::Removed()
{
    Cleanup();

	m_MicrophoneQueue.Cleanup();
	m_HeadphoneQueue.Cleanup();

	m_bChannel = 0;
	m_bPlayerNo = 0;
    m_bCommActive = FALSE;

    DebugPrint("Communicator removed!\n");
}


//-----------------------------------------------------------------------------
// Name: EncodeXMP 
// Desc: Compresses a PCM buffer and returns the encoded buffer and a status on voice detected
//-----------------------------------------------------------------------------
HRESULT CCommunicator::EncodeXMP(LPXMEDIAPACKET pSrcXMP, LPXMEDIAPACKET pDstXMP , BOOL *pvbVoiceDetected)
{
    HRESULT hr;
    DWORD dwSizeDst = 0; 

    memset(pDstXMP->pvBuffer, 0 , pDstXMP->dwMaxSize);

    pDstXMP->pdwCompletedSize = &dwSizeDst;
    pDstXMP->hCompletionEvent = NULL;

    //
    // Call encoding xmo - it's synchronous
    //
    hr = m_pEncoder->Process( pSrcXMP , pDstXMP);

    assert(SUCCEEDED(hr));
    
    /*
	
	  VAD & stuff

	if( dwSizeDst < m_wCodecBufferSize )
    {
        *pvbVoiceDetected = FALSE;
    }
    else
    {
        *pvbVoiceDetected = TRUE;
    }
	
	*/

    pDstXMP->pdwCompletedSize = NULL;

    return( hr );
}

LPXMEDIAPACKET CCommunicator::GetMicrophoneOutput()
{
	if( NULL != m_pMicrophone )
    {
		LPXMEDIAPACKET pXMP = m_MicrophoneQueue.GetCurrentXMP();

		if( XMEDIAPACKET_STATUS_PENDING != *(pXMP->pdwStatus) )
		{
			return (pXMP);
		}
    }
	
	return (NULL);
}

VOID CCommunicator::EnqueueMicrophoneBuffer(LPXMEDIAPACKET pMicrophoneXMP)
{
	assert(NULL != pMicrophoneXMP);

	//
    // Requeue microphone packet - submit the buffer
    //
	HRESULT hr = m_pMicrophone->Process(NULL, pMicrophoneXMP);

	assert(SUCCEEDED(hr));
}

VOID CCommunicator::EnqueueHeadphoneBuffer(LPXMEDIAPACKET pHeadphoneXMP)
{
	assert(NULL != pHeadphoneXMP);

	//
    // Requeue headphone packet - submit the buffer
    //
	HRESULT hr = m_pHeadphone->Process(NULL, pHeadphoneXMP);

	assert(SUCCEEDED(hr));
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
CCommunicatorMgr::CCommunicatorMgr() : m_hMicrophoneWorkEvent(NULL),
                                       m_hNetworkPacketReadyEvent(NULL),
                                       m_pbNetworkPacket(NULL),
									   m_dwNetworkPacketSize(0),
									   m_dwNetworkPacketIndex(0)
{
	for( BYTE i = 0 ; i < COMMUNICATOR_COUNT ; i++)
    {
		m_bEncodePriorityArray[i] = i;
    }		

	m_CodecMediaBuffer.pvBuffer = NULL;
}

//-----------------------------------------------------------------------------
// Name: ~CCommunicatorMgr
// Desc: Destructor
//-----------------------------------------------------------------------------
CCommunicatorMgr::~CCommunicatorMgr()
{
    //
    // The microphone event is for internal use - close handle
    //
	if (m_hMicrophoneWorkEvent) 
    {
        CloseHandle(m_hMicrophoneWorkEvent);
    }

	//
    // m_hNetworkPacketReadyEvent is provided by the client, don't close the handle
    //

	//
    // Delete the network packet allocated by the mgr
    //
	if( NULL != m_pbNetworkPacket )
    {
		delete [] m_pbNetworkPacket;
    }

	//
    // The codec buffer is actually a pointer in the network buffer - don't delete
    //

}

//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Called when the voice chat is initiated on this xbox
//-----------------------------------------------------------------------------
HRESULT CCommunicatorMgr::Initialize(LPVOICE_CHAT_CONFIG pVoiceChatConfig)
{

    HRESULT hr = S_OK;

	assert(NULL != pVoiceChatConfig);

	
	//
    // Initialize the config structure
    //
	m_VoiceParams.Initialize(pVoiceChatConfig);
	
	do
	{
		//
		// Create the work event for the voice units
		//
		m_hMicrophoneWorkEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if( NULL ==  m_hMicrophoneWorkEvent)
		{
			hr = E_FAIL;
			break;
		}

		//
		// Verify the initial state of the voice devices
		//
		hr = CheckInitialState();

		if( FAILED( hr ) )
        {
			break;
        }

		//
        // Allocate buffer for outgoing network packet
        //
		m_dwNetworkPacketSize = CODEC_BUFFER_SIZE * m_VoiceParams.GetMaxEncoders();
		DWORD dwRows = m_VoiceParams.GetSendBufferingTime() / m_VoiceParams.GetPCMBufferTime();
		if( 0 != m_VoiceParams.GetSendBufferingTime() % m_VoiceParams.GetPCMBufferTime() )
        {
			dwRows++;
        }
		m_dwNetworkPacketSize *= dwRows;
		
		m_pbNetworkPacket = new BYTE[m_dwNetworkPacketSize];
		if( NULL == m_pbNetworkPacket )
        {
			hr = E_FAIL;
			break;
        }

	}
	while(FALSE);

	return( hr );
        
}

//-----------------------------------------------------------------------------
// Name: ProcessMicrophones
// Desc: Called in each Process() to check if there is some work TBD for microphones
//-----------------------------------------------------------------------------
HRESULT CCommunicatorMgr::ProcessMicrophones()
{
	HRESULT hr = S_OK;
    BOOL bVoiceDetected = TRUE;
	LPXMEDIAPACKET pMicrophoneXMP = NULL;
	DWORD dwEncodeOps = 0;
    
	
	for( DWORD i = 0 ; (i < COMMUNICATOR_COUNT) && (dwEncodeOps < m_VoiceParams.GetMaxEncoders()) ; i++ )
    {
		//
        // Get the microphone output from 
        //
		pMicrophoneXMP = m_CommArray[m_bEncodePriorityArray[i]].GetMicrophoneOutput();

		if( NULL != pMicrophoneXMP )
        {
            if( SUCCEEDED( *(pMicrophoneXMP->pdwStatus) ) )
			{
				//
                // Assign space in the network buffer for encoding
                //
				m_CodecMediaBuffer.pvBuffer = m_pbNetworkPacket + m_dwNetworkPacketIndex;

				//
                // Update index in the network packet
                //
                m_dwNetworkPacketIndex += CODEC_BUFFER_SIZE;
				
				//
                // Encode PCM data
                //
                hr = m_CommArray[m_bEncodePriorityArray[i]].EncodeXMP(pMicrophoneXMP, &m_CodecMediaBuffer , &bVoiceDetected);

				//
                // If ok, change the priorities for future
                //
				if( SUCCEEDED( hr ) )
                {
				    BYTE bTemp = m_bEncodePriorityArray[dwEncodeOps];
                    m_bEncodePriorityArray[dwEncodeOps] = m_bEncodePriorityArray[i];
                    m_bEncodePriorityArray[i] = bTemp;
                }
				else
				{
					//
                    // Reassign space in the network buffer
                    //
                    m_dwNetworkPacketIndex -= CODEC_BUFFER_SIZE;
				}

				//
                // If network packet is overflown, write from the beginning
                //
				if( m_dwNetworkPacketIndex >= m_dwNetworkPacketSize)
                {
					m_dwNetworkPacketIndex = 0;
                }

				//
                // Count the encoding operation so that we stay in a certain CPU limit
                //
				dwEncodeOps++;

            } // end if

			//
            // Put the packet back in row to be processed by the microphone
            //
			m_CommArray[m_bEncodePriorityArray[i]].EnqueueMicrophoneBuffer(pMicrophoneXMP);
        }
    }

    return (hr);
}

/*
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


    //
    // One of the voice units has microphone input
    //
    if( WAIT_OBJECT_0 == dwReason )
    {
		for( i = 0 ; i <  COMMUNICATOR_COUNT ; i++ )
        {
			hr = m_pVoiceUnits[i]->OnMicrophoneInput();
		    if(FAILED(hr))
			{
                DebugPrint("CCommunicatorMgr::Process()...Error OnMicrophoneInput = %d\n" , hr);
			    return (hr);
			}
        }
	    
    }
			
	//
    // Check for output data even if no microphone input(read the network for example)
    //
	for( i = 0 ; i <  COMMUNICATOR_COUNT ; i++ )
    {
		m_pVoiceUnits[i]->CheckForOutputData();
    }
	
    //
    //  Always check for hot-plugs
    //
    return ( CheckForHotPlugs(msgCommunicatorStatus) );

}
*/

//-----------------------------------------------------------------------------
// Name: CheckInitialState
// Desc: Get the initial state of the devices plugged in, called in Initialize()
//-----------------------------------------------------------------------------
HRESULT CCommunicatorMgr::CheckInitialState()
{
    DWORD dwConnectedMicrophones;
    DWORD dwConnectedHeadphones;
    
    HRESULT hr = S_OK;
    
	do
	{
		//
		//  Get the initial connected state of the devices
		//
		dwConnectedMicrophones = XGetDevices(XDEVICE_TYPE_VOICE_MICROPHONE);
		dwConnectedHeadphones = XGetDevices(XDEVICE_TYPE_VOICE_HEADPHONE);

		if( dwConnectedMicrophones != dwConnectedHeadphones)
		{
			DebugPrint( "CCommunicatorMgr::CheckInitialState()...dwConnectedMicrophones != dwConnectedHeadphone\n");
			hr = E_FAIL;
			break;
		}

		if((0 == dwConnectedMicrophones) || (0 == dwConnectedHeadphones))
		{
			DebugPrint( "CCommunicatorMgr::CheckInitialState()... No Xbox Communicator found!\n");
			hr = E_FAIL;
			break;
		}

		for(BYTE i = 0 ; i < COMMUNICATOR_COUNT ; i++ )
		{
			if(dwConnectedMicrophones&(1<<i))
			{
				if(!SUCCEEDED(m_CommArray[i].Inserted(&m_VoiceParams, i, m_hMicrophoneWorkEvent)))
				{
					DebugPrint( "CCommunicatorMgr::CheckInitialState()...Voice unit initialization failed!\n");
					hr = E_FAIL;
					break;
				}
				else
				{
					DebugPrint( "CCommunicatorMgr::CheckInitialState()...Voice unit %d initialized.\n" , i);
				}
			}
		}
	}
	while(FALSE);
    
    return( hr );
}


//-----------------------------------------------------------------------------
// Name: CheckForHotPlugs
// Desc: Called in Process to check if new inserts/removes occured
//-----------------------------------------------------------------------------
HRESULT CCommunicatorMgr::CheckForHotPlugs()
{
    DWORD dwMicrophoneInsertions, dwHeadphoneInsertions;
    DWORD dwMicrophoneRemovals, dwHeadphoneRemovals;

    HRESULT hr = S_OK;

    do
	{
		if(XGetDeviceChanges(XDEVICE_TYPE_VOICE_MICROPHONE, &dwMicrophoneInsertions, &dwMicrophoneRemovals))
		{
			if(!XGetDeviceChanges(XDEVICE_TYPE_VOICE_HEADPHONE, &dwHeadphoneInsertions, &dwHeadphoneRemovals))
			{
				DebugPrint("CCommunicatorMgr::CheckForHotPlugs()...GetDeviceChanges does not match\n");
				hr = E_FAIL;
				break;
			}

			if( (dwMicrophoneInsertions != dwHeadphoneInsertions) ||
				(dwMicrophoneRemovals != dwHeadphoneRemovals))
			{
				DebugPrint("CCommunicatorMgr::CheckForHotPlugs()...GetDeviceChanges does not match, different\n");
				hr = E_FAIL;
				break;
			}

			//
			//  Loop over all the ports
			//
			for(BYTE i = 0 ; i < COMMUNICATOR_COUNT ; i++ )
			{
				//
				//  Handle Removal, if there is one.
				//
				if(dwMicrophoneRemovals&(1<<i))
				{
					m_CommArray[i].Removed();

					if( FAILED( hr ) )
					{
						break;
					}

				}

				//
				//  Handle Insertion, if there is one.
				//
				if(dwMicrophoneInsertions&(1<<i))
				{
					hr = m_CommArray[i].Inserted(&m_VoiceParams, i, m_hMicrophoneWorkEvent);

					if( FAILED( hr ) )
					{
						break;
					}
					
				}
			} //end of loop over ports

		}  //end of check for getting device changes
	}
	while(FALSE);
	
    return (hr);
}
